# Semaphores

## Initializing Git on local machine

1. Initialize directory on local machine
	git init
2. Add remote branch tracking
	git remote add origin https://github.com/drewstiles/Semaphores.git
3. Create a branch off master for workspace
	git pull origin master  
	*local master branch up-to-date with remote master branch*  
	git checkout -b a-branch-name  
	*new branch created and switched to*  


**Updating local branch**
	git add .  
	*files added to staging phase*  
	git commit -m "a commit message"  
	*staged files bookmarked in time*  
	git log --oneline  
	*chronological list of bookmarks*  
	
**Updating server with your local branch status**
	git pull origin your-branch-name  
	*make sure your branch up-to-date with remote version*  
	git push origin your-branch-name  
	*pushes the most recent bookmark to server*  


