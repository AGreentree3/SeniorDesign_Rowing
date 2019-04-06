%CatchTiming.m
%Program to test identifying the catch, feather, square from the sensor data
%March 21, 2019
%Team FrEE SpEEd
%% Read File, parse data, filter
clear all
clf

 M = csvread('WFeather_12_4_18.txt'); % Read in all raw data
 M2 = csvread("10NOFeather.txt");

%M = csvread('featherandsquare_3_26.txt'); % Read in all raw data

%Parse Data
xg = M(:,4);
yg = M(:,5);
zg = M(:,6);
xa = M(:,1);
ya = M(:,2);
za = M(:,3);


xg2 = M2(:,4);
yg2 = M2(:,5);
zg2 = M2(:,6);
xa2 = M2(:,1);
ya2 = M2(:,2);
za2 = M2(:,3);

%Remove start data since gyro values are zero
xg = xg(100:length(xg)-100);
yg = yg(100:length(yg)-100);
zg = zg(100:length(zg)-100);
xa = xa(100:length(xa)-100);
ya = ya(100:length(ya)-100);
za = za(100:length(za)-100);

xg2 = xg2(100:length(xg2));
yg2 = yg2(100:length(yg2));
zg2 = zg2(100:length(zg2));
xa2 = xa2(100:length(xa2));
ya2 = ya2(100:length(ya2));
za2 = za2(100:length(za2));

% xg = [xg2;xg;xg2];
% yg = [yg2;yg;yg2];
% zg = [zg2;zg;zg2];
% xa = [xa2;xa;xa2];
% ya = [ya2;ya;ya2];
% za = [za2;za;za2];

% Generate time vector
readsPerSecond=100;
t = linspace(1,length(xg),length(xg))/readsPerSecond;

%% Create figure
figure(1)
hold on
grid on
axis([0, max(t), -1, 2])
plot(t, ya/10, 'k-', t, zg, 'b-', t, xg/10, 'g-')

%% Initialize variables
numCatches=0;
numSquares=0;
numFeathers=0;
featheringcount=0; %number of data points in state feather
nonfeatheringcount=0; %number of data points in state non feather
strokeSecs = 2; %seconds per stroke, used for sl
ready4NewCatch=0;
timeChange=0;
featheringState=0; 
featherTimer=0;
reSquaredForCatch=1;

if(ya(1)>6)
    featherOrSquare=1; %squared state
else
    featherOrSquare=0; %feathered state
end

%% Input data and analyze
%On plot, red line is catch, magenta line is feather, cyan line is square
for i=1:length(t)
    
    %featheringState = whether feathering is happening
    %featherOrSquare = 0 blade is parallel to water, 1 blade is
    %perpendicular to water
    
    %Determine feathering state
    if(xg(i)<-1)
        featheringState = 1;
        featherTimer=0; %reset timer when we feather again
    end
    
    if(featheringState==1)
        featheringcount=featheringcount+1; %just for testing, remove for C
        featherTimer=featherTimer+1; %counts reads since last feathering motion 
        if(featherTimer>readsPerSecond*3.5)
            featheringState=0; %no longer feathering
            featherTimer=0;
        end
    else
        nonfeatheringcount=nonfeatheringcount+1; %just for testing, remove for C
    end
    
    
    %State machine for featherOrSquare
    if(xg(i)<-3  && featherOrSquare==1) %&& ya(i)<10
        featheringState=1;
        featherOrSquare=0;
        plot([t(i),t(i)],[-8,6],'m-')
        drawnow
        numFeathers=numFeathers+1
    elseif(xg(i)>1.5 && ya(i)>6 && featherOrSquare==0)
        featheringState=1;
        featherOrSquare=1;
        reSquaredForCatch=1; %for catch state machine
        plot([t(i),t(i)],[-8,6],'c-')
        drawnow
        numSquares=numSquares+1
    end
    
    
    %State machine for catches    
    if(ready4NewCatch==0) %%not ready for catch
        timeChange=timeChange+1;
        if(timeChange>readsPerSecond*1)
            ready4NewCatch=1;
            timeChange=0;
        end
        
    elseif(ready4NewCatch==1) %ready for catch
        %criteria depend on if we're feathering or not
        
        if(featheringState==1) 
             %if(zg(i)<-0.2 &&(mean(ya(i-readsPerSecond:i-readsPerSecond/2))<7) )
                 %I don't love the moving average approach done here
             if(zg(i)<-0.2 && reSquaredForCatch==1 )%&&featherOrSquare==1)
                 plot([t(i),t(i)],[-8,6],'r-')
                 drawnow
                 ready4NewCatch=0;
                 numCatches=numCatches+1
                 reSquaredForCatch=0;
             end
            
        elseif(featheringState==0 && zg(i)<-0.18)
             plot([t(i),t(i)],[-8,6],'r-')
             drawnow
             ready4NewCatch=0;
             numCatches=numCatches+1
        end
    end
    
end

legend('Y accel/10', 'Z gyro', 'X gyro/10');
hold off

%% Method 2: Maintain Value for sustained period of time

%% Method 3: Check Average Value
