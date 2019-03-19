clear all

M = csvread("10NOFeather.txt");
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

subplot(3,3,1)
plot(t,ivec)
subplot(3,3,2)
plot(t,jvec)
subplot(3,3,3)
plot(t,kvec)

subplot(3,3,4)
plot(t,xg)
subplot(3,3,5)
plot(t,yg)
subplot(3,3,6)
plot(t,zg)

subplot(3,3,7)
plot(t,xa)
subplot(3,3,8)
plot(t,ya)
subplot(3,3,9)
plot(t,za)

figure(2)
plot(t,ya,t,za)
legend('y','z')