@echo on 

REM 安装驱动
sc create booster type= kernel binPath=C:\Work\threadPriority\Debug\DriverBooster.sys


REM 启动驱动
sc start booster 