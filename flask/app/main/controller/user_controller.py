from flask_cors import cross_origin, CORS
from mysql.connector import IntegrityError

import app.main.model.user as user
from app.main.service import user_service
from flask import Blueprint, request, jsonify

user_bp = Blueprint('user', __name__)
CORS(user_bp)


@user_bp.route("/users", methods=['POST'])
@cross_origin()
def create_user():
    data = request.json
    username = data.get('username')
    password = data.get('password')

    if not password or not username:
        return jsonify({'error': 'Username and password required'}), 400

    if user_service.get_user_by_uname(username):
        return jsonify({"error": "USER_ALREADY_EXISTS"}), 409

    user = user_service.create_user(username, password)
    return jsonify({"access_token": user._access_token}), 201


@user_bp.route("/users", methods=['GET'])
def get_users():
    users = user_service.getall()
    users_list = [{"username": user.username} for user in users]
    return jsonify(users_list), 200

@user_bp.route("/users/info", methods=['POST'])
@cross_origin()
def get_user_info():
    data = request.json
    access_token = data.get("access_token")
    current_user = user_service.get_user_by_token(access_token)

    if current_user is None:
        return jsonify({"error": "invalid token"}), 401

    return jsonify({
        "username": current_user.username,
        "privileges": "User"
    }), 200

@user_bp.route("/users/get_token", methods=['POST', 'OPTIONS'])
@cross_origin()
def get_token():
    data = request.json
    username = data.get("username")
    password = data.get("password")

    token = user_service.get_token(username, password)

    if token is not None:
        response = jsonify({"access_token": token})
        return response, 200
    return jsonify({}), 401


@user_bp.route("/users/delete", methods=['DELETE'])
@cross_origin()
def delete():
    data = request.json
    access_token = data.get("access_token")
    user_service.delete_user(user_service.get_user_by_token(access_token))
    return jsonify({}), 200


@user_bp.route("/users/change_username", methods=['PATCH'])
@cross_origin()
def change_username():
    data = request.json
    new_username = data.get("new_username")
    access_token = data.get("access_token")
    try:
        user_service.update_user_uname(new_username, access_token)
    except IntegrityError:
        return jsonify({"error": "Username is already in use"}), 409
    except ValueError:
        print(access_token)
        return jsonify({"error": "Wrong authentication token"}), 401
    return jsonify({}), 200


@user_bp.route("/users/change_password", methods=['PATCH'])
def change_password():
    data = request.json
    new_password = data.get("new_password")
    access_token = data.get("access_token")
    try:
        user_service.update_user_pwd(new_password, access_token)
    except ValueError as e:
        print(e.__str__())
        return jsonify({"error": "User doesn't exist or is not authenticated properly"}), 401
    return jsonify({}), 200


@user_bp.route("/users/regen_token", methods=['PATCH'])
def regen_token():
    data = request.json
    access_token = data.get("access_token")
    try:
        new_token = user_service.regen_token(access_token)
    except ValueError:
        return jsonify({"error": "User doesn't exist or is not authenticated properly"}), 401
    return jsonify({"access_token": new_token}), 200


