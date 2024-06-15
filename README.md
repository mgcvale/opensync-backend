# Hyprsync

Um sistema de backup de imagens open-source e cross-plataform.
Desenvolvido em Python, utilizando Flask em uma API RESTful

# Instrucoes para instalacao/utilizacao

Para rodar o servidor, é necessário ter um ambiente python (venv, por exemplo) com os pacotes especificados no arquivo requirements.txt.
Isso poderá ser feito criando um ambiente virtual do zero, ou utilizando uma IDE, como o PyCharm, para facilitar o processo.

Aqui está um exemplo de criacão de um .venv:

Clonar o repositório e entrar na fonte do código
```
git clone https://github.com/mgcvale/hyprsync-backend
cd hyprsync-backend/flask
```
Criar o venv e instalar os requirements
``` shell
python3 -m venv venv
# No windows:
venv\Scripts\activate

# No Linux/macOS:
source venv/bin/activate

# instalar os pacotes necessários
pip install -r requirements.txt
```
Após a criacão do servidor, é preciso configurar duas coisas: o banco de dados e as configuracões locais do servidor.

## Configuracoes Locais:

As duas configuracões que você precisará mudar estão localizadas em dois arquivos: um deles é o .env, e o outro é o config.py.
O arquivo .env deverá ser criado na pasta /flask/app. Nele, você precisará setar as seguintes variáveis, da seguinte forma:

``` python
# File: /flask/app/.env

# database
MYSQL_HOST = "192.168.10.12" # ip do banco de dados
MYSQL_USER = "user" # nome de usuário do banco de dados
MYSQL_DB = "bd_schema" # bd (schema) do banco de dados
MYSQL_PASSWORD = "senha" # senha para o usuário do banco de dados

# file storage
USER_FILES_DIR = "/home/mgcv/hyprsync/user_uploads" # pasta em que os arquivos que o servidor receber dos usuários serão armazenados. IMPORTANTE: A pasta que você colocar aqui deverá existir no seu computador, pois o código não criará ela para você, caso ela não exista.
```

Já o arquivo config.py já vem criado, e está localizado na pasta /flask. A única configuracão que você deverá trocar nele será a UPLOAD_FOLDER, que deverá ser IGUAL à USER_FILES_DIR no arquivo .env.
```python
# File: /flask/config.py

import os

basedir = os.path.abspath(os.path.dirname(__file__))

class Config:
    SECRET_KEY = os.environ.get('SECRET_KEY')
    UPLOAD_FOLDER = "/home/mgcv/hyprsync/user_uploads" # trocar esse diretório pelo mesmo que você colocou em USER_FILES_DIR, no arquivo .env.
    PREVIEW_FOLDER = "preview/"
    DEBUG = False
```

## Banco de dados:

O Banco de dados utilizado no projeto foi o MySql/MariaDB.
Após ter colocado os dados do seu BD no arquivo .env, você deverá criar uma única tabela `user`, com os seguintes campos:
```
+---------------+-------------+------+-----+---------+----------------+
| Field         | Type        | Null | Key | Default | Extra          |
+---------------+-------------+------+-----+---------+----------------+
| id            | int(11)     | NO   | PRI | NULL    | auto_increment |
| username      | varchar(45) | YES  | UNI | NULL    |                |
| password_hash | varchar(64) | YES  |     | NULL    |                |
| access_token  | varchar(45) | YES  |     | NULL    |                |
| salt          | varchar(64) | YES  |     | NULL    |                |
+---------------+-------------+------+-----+---------+----------------+
```
Aqui está um código para criar esta tabela:
```sql
CREATE TABLE user (
    id INT(11) NOT NULL AUTO_INCREMENT,
    username VARCHAR(45) UNIQUE,
    password_hash VARCHAR(64),
    access_token VARCHAR(45),
    salt VARCHAR(64),
    PRIMARY KEY (id)
);
```

Após ter feito tudo isso, inicie o servidor com `flask run`.

Caso você encontre algum problema, sinta-se livre em abrir uma issue no github.
As instrucões para uso do frontend (WEB) estão no readme do repositório do mesmo: https://github.com/mgcvale/hyprsync-frontend
