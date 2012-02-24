all:
	node-waf -v configure 
	node-waf -v build 
install:
	NODE_PATH=/ node-waf --prefix=/ --destdir=/home/rngadam/lophilo-bin install 
clean:
	node-waf -v clean
