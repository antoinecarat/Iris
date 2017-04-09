# Iris
###### School project, network course. Nantes, France.

Iris is a simple version control system. This is how to use it:

## Creating a project, work and save it on server.

### Step 1: Start to work on something and create your own repository.
If you want to use iris, you first have to create the global architecture, to do so use the following command:

##### iris init

Then, put your files into ___iris/projects/ *your-project-name* /___ and use the following command to create your own repository on the server:

##### iris create *your-project-name* *server-adress* *server-port* *your-name*

### Step 2: Make your working space clean
Now you've got a repository on the server, but to start using iris correctly, you must remove your local files and clone your repository from server.
To do so, the following commands would be useful:

##### rm -rf iris/projects/*your-project-name*
##### iris clone *your-project-name* *server-adress* *server-port* *your-name*

### Step 3: Work and notify iris 
When you do something, you must notify iris, so it can create a version note when you'll push your work.
To notify that you've done something on a file in a project:

#### iris add *project-name* *filename*
#### iris mod *project-name* *filename*
#### iris del *project-name* *filename*

### Step 4: Save your work on server
Time has come to save your work on server. If you're sure that you've notify all your modifications, use this command to save your work on server:

##### iris push *your-project-name* *server-adress* *server-port* *your-name*

## Retrieving someone's work from server.

If you want to retrieve a project from the server, just enter the following:

##### iris clone *your-project-name* *server-adress* *server-port* *your-name*

If you've already worked on this project, enter the following:

##### iris pull *your-project-name* *server-adress* *server-port* *your-name*

## Managing server

If you want to create an iris server, you'll have to initialize the architecture first, to do so:

#### iris-server init

Once you've done this, you can start listening network, seeking for client:

#### iris-server listen

N.B.: The listening port is 2500.

You can also list all the available projects:

#### iris-server list-projects