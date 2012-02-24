
NODE_WAF_PARAMS=--verbose --prefix=/ --destdir=/home/rngadam/lophilo-bin --blddir=/home/rngadam/lophilo-builds/node-lophilo
all:
	NODE_PATH=/ node-waf $(NODE_WAF_PARAMS) configure 
	NODE_PATH=/ node-waf $(NODE_WAF_PARAMS) build 
	NODE_PATH=/ node-waf $(NODE_WAF_PARAMS) install 
clean:
	node-waf clean
