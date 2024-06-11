import json
from os.path import isfile, join

from dotenv import load_dotenv
from flask import Blueprint, request, jsonify, current_app
from flask_cors import CORS, cross_origin
from werkzeug.utils import secure_filename

from app.main.service import user_service
import os

file_bp = Blueprint('file', __name__)
CORS(file_bp)


ALLOWED_EXTENSIONS = {'png', 'jpeg', 'jpg'}


def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS


@file_bp.route("/files", methods=['POST'])
@cross_origin()
def get_files():
    data = request.json
    access_token = data.get("access_token")

    user = user_service.get_user_by_token(access_token)
    if user is None:
        return jsonify({"error": "User Not Found"}), 401

    load_dotenv()
    userdir = os.path.join(current_app.config['UPLOAD_FOLDER'], user.username)
    userfiles = [f for f in os.listdir(userdir) if isfile(join(userdir, f))]

    return jsonify({"files": json.dumps(userfiles)}), 200


@file_bp.route("/files/upload", methods=['POST'])
@cross_origin()
def upload_files():
    access_token = request.form.get("access_token")
    print(access_token)
    user = user_service.get_user_by_token(access_token)
    if user is None:
        return jsonify({"error": "User Not Found"}), 401
    filedir = os.path.join(current_app.config["UPLOAD_FOLDER"], user.username)
    files = request.files.getlist('files')
    for f in files:
        print(f.filename)
        filename = secure_filename(f.filename)
        if not allowed_file(filename):
            return jsonify({"error": "File type not allowed"}), 400
        f.save(os.path.join(filedir, filename))
    return jsonify({}), 200


@file_bp.route("/files/download", methods=['POST'])
@cross_origin()
def download_files():
    return None


