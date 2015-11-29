# Semaphores

**Initializing Git on local machine**
  
  	// establish git version control within current directory
	git init 
	 
	git remote add origin https://github.com/drewstiles/Semaphores.git  
	 
	// bring local master branch up-to-date with remote master branch  
	git pull origin master  
	
	// create and switch to new branch
	git checkout -b a-branch-name   


**Updating local branch**  

	// add files to be bookmarked next
	git add .    

	// create a bookmark of directory status
	git commit -m "a commit message"   
	
	// get chronological list of bookmarks 
	git log --oneline   
	
**Updating server with your local branch status**  

	// bring local branch up-to-date with remote version 
	git pull origin your-branch-name   

	// push the most recent bookmark to server	
	git push origin your-branch-name  
  


