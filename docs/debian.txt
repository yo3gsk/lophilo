using apt-cacher-ng

packages are stored in /var/www/repos/

= package signing =

rngadam@cnshaqs10:~$ gpg --list-keys
/home/rngadam/.gnupg/pubring.gpg
--------------------------------
pub   2048R/37FC6E55 2012-03-13
uid                  Ricky Ng-Adam (For signing Debian packages) <rngadam@lophilo.com>
sub   2048R/A84DD3F8 2012-03-13



= client =ls


Debian:

wget -O - http://cnshaqs10:3142/lophilo/apt/debian/conf/lophilo-debrepo.gpg.key|apt-key add -
echo "deb http://cnshaqs10:3142/lophilo/apt/debian squeeze main" > /etc/apt/sources.list.d/lophilo.list

Ubuntu:
wget -O - http://cnshaqs10:3142/lophilo/apt/ubuntu/conf/lophilo-debrepo.gpg.key|apt-key add -
echo "deb http://cnshaqs10:3142/lophilo/apt/ubuntu oneiric main" > /etc/apt/sources.list.d/lophilo.list

in /etc/apt/sources.list.d/lophilo.list


apt-get update 
apt-get install lophilo-nodejs