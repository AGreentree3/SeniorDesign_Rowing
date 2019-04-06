%CatchTiming.m
%Program to test identifying the catch, feather, square from the sensor data
%March 21, 2019
%Team FrEE SpEEd
%% Read File, parse data, filter
clear all
clf

%M = csvread('WFeather_12_4_18.txt'); % Read in all raw data
M = csvread("10NOFeather.txt");

%Parse Data
xg = M(:,4);
yg = M(:,5);
zg = M(:,6);
xa = M(:,1);
ya = M(:,2);
za = M(:,3);

%Remove start data since gyro values are zero
xg = xg(100:length(xg)-100);
yg = yg(100:length(yg)-100);
zg = zg(100:length(zg)-100);
xa = xa(100:length(xa)-100);
ya = ya(100:length(ya)-100);
za = za(100:length(za)-100);

% Generate time vector
readsPerSecond=100;
t = linspace(1,length(xg),length(xg))/readsPerSecond;

%% Create figure


%% Initialize variables
deltat = 1/readsPerSecond;
x(1)=0;
vx(1)=0;
y(1)=0;
vy(1)=0;
z(1)=0;
vz(1)=0;
ax = xa;
ay = ya;
az = za;

%% Input data and analyze

% ax = ax - mean(ax);
% ay = ay - mean(ay);
% az = az - mean(az);

for n=1:length(ax)-1
    %Let's use the verlet velocity algorithm
    %http://www.physics.drexel.edu/~valliere/PHYS305/Diff_Eq_Integrators/Verlet_Methods/Verlet/Verlet.html
    vx_half = vx(n)+ax(n)/2*deltat;
    x(n+1)=x(n)+vx_half*deltat;
    vx(n+1)=vx_half+ax(n+1)/2*deltat;
    
    vy_half = vy(n)+ay(n)/2*deltat;
    y(n+1)=y(n)+vy_half*deltat;
    vy(n+1)=vy_half+ay(n+1)/2*deltat;
    
    vz_half = vz(n)+az(n)/2*deltat;
    z(n+1)=z(n)+vz_half*deltat;
    vz(n+1)=vz_half+az(n+1)/2*deltat;
    
    %Cheating to keep at zero
%     if(abs(vx(n+1))<0.03 && ax(n+1)<0)
%         x(n+1)=0;
%     end
end

figure(1)
plot(t, ax,'k-',t,vx,'b-',t,x,'g-')
legend('ax', 'vx', 'x');
figure(2)
plot(t, ay,'k-',t,vy,'b-',t,y,'g-')
legend('ay', 'vy', 'y');
figure(3)
plot(t, az,'k-',t,vz,'b-',t,z,'g-')
legend('az', 'vz', 'z');

hold off

%% Method 2: Maintain Value for sustained period of time

%% Method 3: Check Average Value
