# Contributing

## Project Structure:

The project follows a MVC-equivalent model (Model, Service, Handler).

### Models
The model has structs for each logical model, and functions for its management.
These functions include:

- creation/freeing functions
- management functions, such as model list managers

### Servivces
The service layer contains functions for storing, retrieving, deleting and updating the models on the database.
All the functions retrieve data by reference, and return an integer refering to the exit code. The codes are defined on datbase.h, and you can create a new one if needed.

### Handlers
The handlers are the functions that handle the actual endpoints of the API.

The root-handler is located in the handler.c file. After checking the URI of the request, it redirects it to the root handler of the correct subhandler, which in turn calls the correct final handler for that endpoint.
They follow a fairly standard model, so you can just use the mongoose documentation to implement one.

## Best practices

When addind new functions/endpoints/functionality to the API, some best practices should be followed.
They include:

- verifying memory leaks and unsafe code with debuggers such as valgrind
- checking for null values and other errors in the functions you call
- log errors and important warnings and/or messages using mongoose's logger
