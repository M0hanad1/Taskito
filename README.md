# Taskito

**Taskito** is a simple command-line To-Do list manager written in C.  
It allows you to easily create, view, search, mark, and delete tasks, storing your data in a binary file.

---

## Features

- Add new tasks with a title, description, and done status
- List all tasks in your database
- Search for tasks by keywords in title or description
- Mark tasks as done/undone
- Remove tasks from the database
- Create or delete the entire task database as needed

---

## Releases

You can download the release executable from [here](https://github.com/M0hanad1/Taskito/releases/latest).

And if you want you can build yourself from [here](https://github.com/M0hanad1/Taskito?tab=readme-ov-file#building).

---

## Usage

```sh
./taskito [options]
./bin/taskito [options] # Building output using Make.
```

### Options

| Option          | Description                                               |
| --------------- | --------------------------------------------------------- |
| `-f <filename>` | (Required) Path to the database file                      |
| `-n`            | Create a new database file                                |
| `-a <task>`     | Add a new task: `<title,description,done>` (done: 0 or 1) |
| `-s <query>`    | Search for tasks by keyword                               |
| `-r <query>`    | Remove a task by keyword                                  |
| `-m <query>`    | Mark task as done/undone by keyword                       |
| `-g`            | List all tasks in the database                            |
| `-d`            | Delete the database file                                  |

#### Example: Add a Task

```sh
./taskito -n -f tasks.db               # Create a new database
./taskito -a "Exercise,Go to the gym,0" -f tasks.db
# result: Title: Exercise, Description: Go to the gym, Done: Not done.
```

#### Example: List All Tasks

```sh
./taskito -g -f tasks.db
```

#### Example: Search Tasks

```sh
./taskito -s "exercise" -f tasks.db
```

#### Example: Mark a Task as Done

```sh
./taskito -m "exercise" -f tasks.db
```

#### Example: Remove a Task

```sh
./taskito -r "exercise" -f tasks.db
```

#### Example: Delete the Database

```sh
./taskito -d -f tasks.db
```

---

## Building

You need a C compiler (e.g., gcc) and make.  
To build the project:

```sh
make
```

This will generate the `taskito` executable at the `./bin/` folder.

---

## Code Structure

- `src/` - Source code files
- `include/` - Header files
- `Makefile` - Build instructions

---

## Data Format

Each task consists of:

- Title (max 50 characters)
- Description (max 200 characters)
- Done status (0 = not done, 1 = done)

All data is stored in a binary file specified by `-f`.
