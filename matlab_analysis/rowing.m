clear all
clf

%M = csvread("10NOFeather.txt");
M = csvread('WFeather_12_4_18.txt');

ivec = M(:,7);
jvec = M(:,8);
kvec = M(:,9);
xg = M(:,4);
yg = M(:,5);
zg = M(:,6);
xa = M(:,1);
ya = M(:,2);
za = M(:,3);

t = linspace(1,length(ivec),length(ivec))/100;

figure(1)
subplot(3,3,1)
plot(t,ivec)
title('I pos vec')
subplot(3,3,2)
plot(t,jvec)
title('J pos vec')
subplot(3,3,3)
plot(t,kvec)
title('K pos vec')

subplot(3,3,4)
plot(t,xg)
title('X gyro')
subplot(3,3,5)
plot(t,yg)
title('Y gyro')
subplot(3,3,6)
plot(t,zg)
title('Z gyro')

subplot(3,3,7)
plot(t,xa)
title('X accel')
subplot(3,3,8)
plot(t,ya)
title('Y accel')
subplot(3,3,9)
plot(t,za)
title('Z accel')

%  figure(2)
%  plot(t,zg*10,t,ya)
%  legend('z gyro', 'y acceleration')