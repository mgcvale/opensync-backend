import json
from os.path import isfile, join

from dotenv import load_dotenv
from flask import Blueprint, request, jsonify, current_app, send_file
from flask_cors import CORS, cross_origin
from werkzeug.utils import secure_filename

from app.main.service import user_service
from app.main.service import file_service
import os

file_bp = Blueprint('file', __name__)
CORS(file_bp)


@file_bp.route("/files", methods=['POST'])
@cross_origin()
def get_files():
    data = request.json
    access_token = data.get("access_token")

    user = user_service.get_user_by_token(access_token)
    if user is None:
        return jsonify({"error": "User Not Found"}), 401

    userfiles = file_service.get_files(user)
    return jsonify({"files": json.dumps(userfiles)}), 200


@file_bp.route("/files/upload", methods=['POST'])
@cross_origin()
def upload_files():
    form = request.form
    access_token = form.get("access_token")
    print(access_token)
    user = user_service.get_user_by_token(access_token)
    if user is None:
        return jsonify({"error": "User Not Found"}), 401
    filedir = os.path.join(current_app.config["UPLOAD_FOLDER"], user.username)
    files = request.files.getlist('images')
    return file_service.upload_files(files, filedir)


@file_bp.route("/files/download/<access_token>/<image>", methods=['GET'])
@cross_origin()
def download_files(access_token, image):

    user = user_service.get_user_by_token(access_token)
    if user is None:
        return jsonify({"error": "User Not Found"}), 401
    filedir = os.path.join(current_app.config["UPLOAD_FOLDER"], user.username)
    filepath = os.path.join(filedir, image)
    return send_file(filepath, mimetype="image/png")


@file_bp.route("/files/delete", methods=['DELETE'])
@cross_origin()
def delete_file():
    data = request.json
    access_token = data.get('access_token')
    filename = data.get('filename')

    user = user_service.get_user_by_token(access_token)
    if user is None:
        return jsonify({"error": "User Not Found"}), 401

    file = os.path.join(current_app.config["UPLOAD_FOLDER"], user.username)
    file = os.path.join(file, filename)
    os.remove(file)
    return jsonify({}), 200