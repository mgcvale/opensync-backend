import base64
import io
import os
from os.path import isfile, join

from PIL import Image
from dotenv import load_dotenv
from flask import jsonify, current_app
from werkzeug.utils import secure_filename

from app.main.model import user

ALLOWED_EXTENSIONS = {'png', 'jpeg', 'jpg'}

load_dotenv()

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS


def upload_files(files, filedir):
    for f in files:
        print(f.filename)
        filename = secure_filename(f.filename)
        if not allowed_file(filename):
            return jsonify({"error": "File type not allowed"}), 400
        f.save(os.path.join(filedir, filename))
    return jsonify({}), 200


def get_files(current_user):
    userdir = os.path.join(current_app.config['UPLOAD_FOLDER'], current_user.username)
    filenames = [f for f in os.listdir(userdir) if isfile(join(userdir, f))]
    data = []

    for file in filenames:
        imagedir = os.path.join(userdir, file)
        encoded_image = encode_image(imagedir)
        if encoded_image is None:
            continue
        image = {'name': file, 'image': encoded_image}
        data.append(image)

    return data


def encode_image(image_path):
    with open(image_path, 'rb') as f:
        return base64.b64encode(f.read()).decode('utf-8')


