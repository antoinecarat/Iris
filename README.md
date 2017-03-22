# Iris
######School project, network course. Nantes, France.

Iris is a simple version control system. This is how to use it as a client:

##Step 1: Start to work on something and create your own repository.
If you want to use iris, you first have to create the global architecture, to do so use the following command:

#####iris init

Then, put your files into ___iris/projects/~your-project-name~/___ and use the following command to create your own repository on the server:

#####iris create ~your-project-name~ ~server-adress~ ~server-port~ ~your-name~

##Step 2: Make your working space clean
Now you've got a repository on the server, but to start using iris correctly, you must remove your local files and clone your repository from server.
To do so, the following commands would be useful:

#####rm -rf iris/projects/~your-project-name~
#####iris clone ~your-project-name~ ~server-adress~ ~server-port~ ~your-name~

#Step 3: 