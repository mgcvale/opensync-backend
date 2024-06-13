import io
import os

from PIL import Image
from flask import current_app


def create_preview(image_path, user):
    img = Image.open(image_path)

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
    img.save(new_image, optimize=True, quality=85)
