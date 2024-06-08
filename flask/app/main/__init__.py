from flask import Blueprint
from flask_cors import CORS

bp = Blueprint('main', __name__)
CORS(bp)

from app.main.controller.user_controller import user_bp
bp.register_blueprint(user_bp)