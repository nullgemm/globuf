run:
	cd bin && $(CMD)

clean:
	make/scripts/clean.sh

remotes:
	@echo "# registering remotes"
	make/scripts/git_remotes.sh

github:
	@echo "# sourcing submodules from https://github.com"
	make/scripts/git_github.sh

gitea:
	@echo "# sourcing submodules from personal server"
	make/scripts/git_gitea.sh
