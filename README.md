# Q_Projects
Personal Test Projects


1. OS Ubuntu 24.04.1 LTS SERVER

sudo apt update  
sudo apt upgrade  
sudo reboot  


sudo apt update  
sudo apt install xfce4 xfce4-goodies 1.3g  
sudo apt install tigervnc-standalone-server  

vncserver ##pw settings  
vncserver -list  
vncserver -kill :1  
sudo apt install vim  
vim ~/.vnc/xstartup  

sudo vim /etc/systemd/system/vncserver@.service  

```
[Unit]
Description=Start TigerVNC server at startup  
After=syslog.target network.target

[Service]
Type=simple
User=alan
Group=alan
WorkingDirectory=/home/alan

PIDFile=/home/someuser/.vnc/%H:590%i.pid
ExecStartPre=-/bin/sh -c "/usr/bin/vncserver -kill :%i > /dev/null 2>&1"
ExecStart=/usr/bin/vncserver -fg -depth 24 -geometry 1920x1080 -localhost no :%i
ExecStop=/usr/bin/vncserver -kill :%i

[Install]
WantedBy=multi-user.target  
```

sudo chmod +x /etc/systemd/system/vncserver@.service  
sudo systemctl daemon-reload  

sudo systemctl enable vncserver@1.service  
   21  sudo systemctl start vncserver@1  
   22  sudo systemctl status vncserver@1  


trouble -> 

failed to execute child process "dbus-launch"  
sudo apt install dbus-x11  



# Wxwidget

alan@Rpi3bplus:~$ sudo apt-cache search libwxgt*  
libwxgtk-gl3.2-1t64 - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 gl library runtime)  
libwxgtk-media3.2-1t64 - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 media library runtime)  
libwxgtk-media3.2-dev - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 media library development)  
libwxgtk-webview3.2-1t64 - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 webview library runtime)  
libwxgtk-webview3.2-dev - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 webview library development)  
libwxgtk3.2-1t64 - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 runtime)  
libwxgtk3.2-dev - wxWidgets Cross-platform C++ GUI toolkit (GTK 3 development)  


sudo apt install cmake
cmake  
make  


#samba  
https://makepluscode.tistory.com/entry/%EC%9A%B0%EB%B6%84%ED%88%AC-Ubuntu-2004-%EC%82%BC%EB%B0%94-Samba-%EC%84%A4%EC%A0%95%ED%95%98%EA%B8%B0  


sudo apt install samba  
sudo vim /etc/samba/smb.conf  

```
[Home]
   comment = sharedSamba
   path = <yourpath>
   guest ok = no
   writable = yes
   create mask = 0644
   directory mask = 0755
   valid users = <youruser>
```

sudo smbpasswd -a <USER>  
```
New SMB password:
Retype new SMB password:
Added user <USER>
```
