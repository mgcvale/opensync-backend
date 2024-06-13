import base64
import os
import shutil
import traceback

from flask import current_app
from mysql.connector import IntegrityError

from app.main.model.user import User
from app.main.model.user import hash_password
from app.main.model.user import gensalt
from app.main.model.user import generate_token


# imports all users form db and creates their instances
def dbimport():
    cursor = current_app.extensions['mysql'].connection.cursor()
    cursor.execute("""select * from user""")
    result = cursor.fetchall()
    for row in result:
        User(username=row[1], password_hash=row[2], access_token=row[3], salt=row[4])


def reload_user(username):
    mysql = current_app.extensions['mysql']
    cursor = mysql.connection.cursor()
    cursor.execute("select * from user where username=%s", [username])
    result = cursor.fetchall()[0]
    User.objects.remove(get_user_by_uname(username))
    User(username=result[1], password_hash=result[2], access_token=result[3], salt=result[4])


def create_user(username, password):
    user = User(username=username, password=password, new=True)
    User.objects.append(user)

    mysql = current_app.extensions['mysql']
    cursor = mysql.connection.cursor()
    try:
        cursor.execute(
            "INSERT INTO user (username, password_hash, access_token, salt) VALUES (%s, %s, %s, %s)",
            (user.username, user._password_hash, user._access_token,  base64.b64encode(user._salt).decode('utf-8'))
        )
        mysql.connection.commit()
    finally:
        cursor.close()

    return user


def getall():
    return User.objects


def delete_user(user):
    mysql = current_app.extensions['mysql']
    cursor = mysql.connection.cursor()
    try:
        cursor.execute("delete from user where username=%s", [user.username])
        mysql.connection.commit()
        User.objects.remove(user)
        shutil.rmtree(os.path.join(current_app.config["UPLOAD_FOLDER"], user.username))
    finally:
        cursor.close()


def get_user_by_uname(username):
    for user in User.objects:
        if user.username == username:
            return user
    return None


def get_token(username, password):
    user = get_user_by_uname(username)
    if user is not None:
        return user.get_token(password)
    return None


def get_user_by_token(token):
    for user in User.objects:
        if user._access_token == token:
            return user
    return None


def update_user_uname(new_username, token):
    user = get_user_by_token(token)
    if user is None:
        raise ValueError("Authentication failed")

    mysql = current_app.extensions['mysql']
    cursor = mysql.connection.cursor()
    try:
        cursor.execute("update user set username=%s where access_token=%s", (new_username, token))
        mysql.connection.commit()
        os.rename(os.path.join(current_app.config["UPLOAD_FOLDER"], user.username), os.path.join(current_app.config["UPLOAD_FOLDER"], new_username))
        user.username = new_username
    except Exception as e:
        print(e)
        raise IntegrityError("User Conflict Error")
    finally:
        cursor.close()


def update_user_pwd(new_password, token):
    user = get_user_by_token(token)
    new_salt = gensalt()
    if user is None:
        raise ValueError("Authentication failed")

    mysql = current_app.extensions['mysql']
    cursor = mysql.connection.cursor()
    try:
        password_hash = hash_password(new_password, new_salt)
        cursor.execute(
            "update user set password_hash=%s, salt=%s where access_token=%s",
            (password_hash, base64.b64encode(new_salt).decode('utf-8'), token)
        )
        mysql.connection.commit()
        reload_user(user.username)
    except ValueError as e:
        traceback.print_exception(e)
    finally:
        cursor.close()


def regen_token(token):
    user = get_user_by_token(token)
    new_token = generate_token()

    if user is None:
        raise ValueError("Authentication Failed")

    mysql = current_app.extensions['mysql']
    cursor = mysql.connection.cursor()
    try:
        cursor.execute("update user set access_token=%s where access_token=%s", (new_token, token))
        mysql.connection.commit()
        user._access_token = new_token
    finally:
        cursor.close()
    return new_token
