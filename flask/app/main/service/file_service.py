import base64
import io
import os
from os.path import isfile, join

from PIL import Image
from dotenv import load_dotenv
from flask import jsonify, current_app
from werkzeug.utils import secure_filename

from app.main.model import user
from app.main.util import image_utils

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
        image_utils.create_preview(os.path.join(filedir, filename), filedir)

    return jsonify({}), 200


def get_files(current_user):
    userdir = os.path.join(current_app.config['UPLOAD_FOLDER'], current_user.username)
    filenames = [f for f in os.listdir(userdir) if isfile(join(userdir, f))]
    data = []

    for file in filenames:
        imagedir = os.path.join(userdir, file)
        try:
            encoded_image = image_utils.get_encoded_preview(file, userdir)
            if encoded_image is None:
                continue
        except:
            pass
        image = {'name': file, 'image': encoded_image}
        data.append(image)

    return data
