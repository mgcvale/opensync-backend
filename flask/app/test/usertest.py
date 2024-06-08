from app.main.model.user import User

def test_hash():
    u = User("joao", "123")
    print(u.auth("123"))


test_hash()