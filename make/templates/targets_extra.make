run:
	cd bin && $(CMD)

clean:
	make/clean.sh

remotes:
	@echo "# registering remotes"
	make/git_remotes.sh

github:
	@echo "# sourcing submodules from https://github.com"
	make/git_github.sh

gitea:
	@echo "# sourcing submodules from personal server"
	make/git_gitea.sh
