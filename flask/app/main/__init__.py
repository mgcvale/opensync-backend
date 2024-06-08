from flask import Blueprint

bp = Blueprint('main', __name__)

from app.main.controller.user_controller import user_bp

bp.register_blueprint(user_bp)