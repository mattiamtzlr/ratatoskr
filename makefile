# I forgot how makefiles work, if someone could deal with this that would be great

format:
	@find . -regex '.*\.\(cpp\|hpp\|cu\|cuh\|c\|h\)' -exec clang-format -style=file -i {} \;
