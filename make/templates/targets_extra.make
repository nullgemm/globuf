run:
	cd bin && $(CMD)

clean:
	make/scripts/clean.sh

remotes:
	@echo "# registering remotes"
	make/scripts/git_remotes.sh
