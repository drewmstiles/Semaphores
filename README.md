# Semaphores

**Initializing Git on local machine**
  
	git init
	// establishes git version control within current directory 
	 
	git remote add origin https://github.com/drewstiles/Semaphores.git  
	 
	git pull origin master  
	// local master branch up-to-date with remote master branch  
	
	git checkout -b a-branch-name  
	// new branch created and switched to  


**Updating local branch**  

	// add files to be bookmarked next
	git add .    

	// create a bookmark of direcoty status
	git commit -m "a commit message"   
	
	// get chronological list of bookmarks 
	git log --oneline   
	
**Updating server with your local branch status**  

	// bring local branch up-to-date with remote version 
	git pull origin your-branch-name   

	// push the most recent bookmark to server	
	git push origin your-branch-name  
  


