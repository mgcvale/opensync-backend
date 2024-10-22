#ifndef OPENSYNC_TEST_TESTS_H
#define OPENSYNC_TEST_TESTS_H

void test_addUserShouldReturnSuccess();
void test_addUserShouldReturnConflictWhenUsernameIsTaken();
void test_userLoginSucceedsWhenPasswordIsCorrect();
void test_userLoginFailsWhenPasswordIsIncorrect();
void test_userAuthenticationSuccedsWhenTokenIsCorrect();
void test_userAuthenticationFailsWhenTokenIsIncorrect();
void test_userDeletionSuccedsWhenTokenIsCorrect();
void test_userDeletionFailsWhenTokenIsIncorrect();


#endif //OPENSYNC_TEST_TESTS_H
