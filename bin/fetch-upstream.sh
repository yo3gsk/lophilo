HOST=cnshaqs10
REPOS=`ssh $HOST -C "find  ~/upstream/ -maxdepth 1 -name '*.git' -type d"`
mkdir -p ~/upstream
for r in $REPOS
do
	BASENAME=`basename $r`
	REPO_NAME=${BASENAME%.*}
	TARGET=~/upstream/$REPO_NAME
	if [ ! -d "$TARGET" ]; then
		echo "$TARGET does not exist, cloning"
		git clone ssh://${HOST}${r} ${TARGET}
	else
		echo "$TARGET already exist!"
	fi
done
