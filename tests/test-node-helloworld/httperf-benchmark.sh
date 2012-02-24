# running node webserver.js
sudo httperf --hog --server 10.236.10.145 --port 1337 --num-conns=1000 > httperf-output-` date +%s`.txt
