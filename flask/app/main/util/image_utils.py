import base64
import io
import os

from PIL import Image
from flask import current_app


def create_preview(image_path, filedir):
    img = Image.open(image_path)
    filedir = os.path.join(filedir, current_app.config['PREVIEW_FOLDER'])
    new_image_path = os.path.join(filedir, os.path.basename(image_path).split('/')[-1])

    img.thumbnail((192, 192))
    initial_w, initial_h = img.size

    # Square the iamge
    if initial_w >= initial_h:
        diff = initial_w - initial_h
        h = initial_h
        w = initial_h
        x = diff / 2
        y = 0
    else:
        diff = initial_h - initial_w
        h = initial_w
        w = initial_w
        y = diff / 2
        x = 0
    box = (x, y, x + w, y + h)
    img = img.crop(box)

    # Save and compress image
    if img.mode not in "RGB":
        img = img.convert("RGB")
    img.save(new_image_path, optimize=True, quality=85)


def get_encoded_preview(image_name, filedir):
    preview_path = os.path.join(filedir, current_app.config['PREVIEW_FOLDER'])
    preview_path = os.path.join(preview_path, image_name)

    with open(preview_path, 'rb') as f:
        return base64.b64encode(f.read()).decode('utf-8')

