NODE_WAF_PARAMS=--verbose --prefix=/ --destdir=/home/rngadam/lophilo-bin --blddir=/home/rngadam/lophilo-builds/node-lophilo
all:
	node-waf $(NODE_WAF_PARAMS) configure 
	node-waf $(NODE_WAF_PARAMS) build 
	node-waf $(NODE_WAF_PARAMS) install 
clean:
	node-waf clean
