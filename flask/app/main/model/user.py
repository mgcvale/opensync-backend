import base64
import string

import bcrypt
import secrets


def gensalt():
    return bcrypt.gensalt()


def generate_token():
    return secrets.token_hex(16)


def hash_password(password, salt):
    return bcrypt.hashpw(password.encode('utf-8'), salt)


class User:
    objects = []

    def __init__(self, **kwargs):
        # Check if the user already exists
        for user in User.objects:
            if user.username == kwargs.get("username"):
                raise ValueError("USER_ALREADY_EXISTS")

        self.username = kwargs.get("username")
        self._access_token = kwargs.get("access_token")
        self._password_hash = kwargs.get("password_hash")

        if kwargs.get("new"):
            self._salt = gensalt()
            self._password_hash = hash_password(kwargs.get("password"), self._salt)
            self._access_token = secrets.token_hex(16)
        else:
            self._salt = base64.b64decode(kwargs.get("salt") + "==")

        User.objects.append(self)

    def get_token(self, password):
        hashed_pwd = hash_password(password, self._salt).decode('utf-8')
        return self._access_token if hashed_pwd == self._password_hash else None

    def auth(self, token):
        return token == self._access_token

    def __str__(self):
        return self.username
