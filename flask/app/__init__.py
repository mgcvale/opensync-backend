import os

from flask import Flask, logging
from dotenv import load_dotenv
from flask_mysqldb import MySQL
from flask_cors import CORS, cross_origin

from config import Config
from app.main.service import user_service


def create_app(config_class=Config):
    app = Flask(__name__)
    app.config.from_object(config_class)

    # Initialize db here
    load_dotenv()
    app.config['MYSQL_HOST'] = os.getenv("MYSQL_HOST")
    app.config['MYSQL_USER'] = os.getenv("MYSQL_USER")
    app.config['MYSQL_PASSWORD'] = os.getenv("MYSQL_PASSWORD")
    app.config['MYSQL_DB'] = os.getenv("MYSQL_DB")


    # Initialize Flask extensions here
    mysql = MySQL(app)
    app.extensions['mysql'] = mysql
    with app.app_context():
        user_service.dbimport()

    # Register blueprints here
    from app.main import bp as main_bp
    app.register_blueprint(main_bp)

    # Cors setup

    return app
