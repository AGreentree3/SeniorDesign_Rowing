%CatchTiming.m
%Program to test identifying the catch from the sensor data
%December 6th
%Team FrEE SpEEd
%% Read File and parse data
clear all

%M = csvread('WFeather_12_4_18.txt'); % Read in all raw data
M = csvread("10NOFeather.txt");

%Parse Data
xg = M(:,4);    % x gyroscope
yg = M(:,5);    % y gyroscope
zg = M(:,6);    % z gyroscope
ya = M(:,2);

t = linspace(1,length(xg),length(xg))/100; % Generator time vector

%% Method 1: Set High Peak Value For XGyro
state = 0; % waiting for square up to begin
           %= 1; waiting to for squared
           %=2; waiting for oar drop. ready to identify catch

%Step through gyro values and try to identity catch
figure(1)
hold on
grid on
axis([0, 45, -8, 6])
plot(t, xg, 'k-', t, zg, 'b-')
num=0;

for i=1:length(t)
    %plot(t(i),xg(i),'b.', t(i), zg(i), 'r.');
    %drawnow
     
%     if (xg(i)>2) && (state==0)
%          state = 1; 
%         plot([t(i),t(i)],[-8,6],'b-')
%          drawnow
%     end
%     
%     
%     if (xg(i)<=0) && (state==1)
%         plot([t(i),t(i)],[-8,6],'g-')
%          drawnow
%         state = 2; 
%     end
%      
%      %identifies catch
%      if (zg(i)<=-0.2) && (state==2)
%          num=num+1;
%          plot([t(i),t(i)],[-8,6],'r-')
%          drawnow
%          state=0;
%      end

    if (ya(i)<5 && zg(i)<0)
         plot([t(i),t(i)],[-8,6],'r-')
         drawnow
    end
    
end

legend('X gyro', 'Z gyro');
%This method is not robust enough to deal with data outliers
%Is the catch at the right point?
%Worried about the accrual of time errors. Eventually should do analysis to
%combine all source of time error and determine accuracy of the system as a
%whole. 
%% Method 2: Maintain Value for sustained period of time

%% Method 3: Check Average Value
