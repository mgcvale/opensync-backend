#ifndef OPENSYNC_TEST_TESTS_H
#define OPENSYNC_TEST_TESTS_H

void test_addUserShouldReturnSuccess(void);
void test_addUserConflictsWhenUsernameIsAlreadyTaken(void);
void test_userGetByPasswordSucceedsWhenPasswordIsCorrect(void);
void test_userGetByPasswordFailsWhenPasswordIsIncorrect(void);
void test_userAuthenticationSucceedsWhenPasswordIsCorrect(void);
void test_userAuthenticationFailsWhenPasswordIsIncorrect(void);
void test_userDeletionSuccedsWhenTokenIsCorrect(void);
void test_userDeletionFailsWhenTokenIsIncorrect(void);
void test_userGetByTokenSucceedsWhenTokenIsCorrect(void);
void test_userGetByTokenFailsWhenTokenIsIncorrect(void);
void test_userGetByTokenFailsWhenTokenIsMissing(void);
int empty(void);
int empty2(void);


#endif //OPENSYNC_TEST_TESTS_H
