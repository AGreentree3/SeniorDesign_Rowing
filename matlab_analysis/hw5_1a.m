clear all

x1 = linspace(-0.5,0.5,1000);
x2 = linspace(-0.5,0.5,1000);
[X1,X2] = meshgrid(x1,x2);
Fx = 5*X1.^2+2*X1.*X2+2*X2.^2-2*X1-X2+0.25;

hold on
contour(X1,X2,Fx)
plot(0,1,'.b','MarkerSize',10)

xlabel('x_{1}')
ylabel('x_{2}')
title('Contour Plot of F(x)')
hold off

%% Steepest descent method- Part 1b
clear all
clf

syms x1;
syms x2;
syms F(x1,x2);
initial_guess = [1 1]';
x(:,1) = initial_guess;
F(x1,x2) = 5*x1^2+2*x1*x2+2*x2^2-2*x1-x2+0.25;
Fgrad = gradient(F, [x1,x2]);
Fhess = hessian(F,[x1,x2]);

difference = 1;
k=1;
while difference>0.000001
    g(:,k)=Fgrad(x(1,k),x(2,k));
    p(:,k) = -g(:,k);
    A = double(Fhess(x(1,k),x(2,k)));
    a(k) = -double(g(:,k)'*p(:,k)/(p(:,k)'*A*p(:,k)))
    x(:,k+1)=x(:,k)-a(k)*g(:,k)
    difference = sqrt((x(1,k+1)-x(1,k))^2 + (x(2,k+1)-x(2,k))^2);
    k=k+1;
end

hold on
ezcontour(F,[-1 1])
title('F(x) with Steepest Descent')
x=x';
for i=1:(length(x')-1)
    arrow_length = sqrt((x(i+1,1)-x(i,1))^2 + (x(i+1,2)-x(i,2))^2);
    quiver(x(i,1),x(i,2),(x(i+1,1)-x(i,1)),(x(i+1,2)-x(i,2)),1,'Color','black','MaxHeadSize',100/arrow_length);
end
hold off

%% Newton's method- Part 1c
clear all
clf

syms x1;
syms x2;
syms F(x1,x2);
initial_guess = [1 1]';
alpha = 0.01;
x(:,1) = initial_guess;
F(x1,x2) = 5*x1^2+2*x1*x2+2*x2^2-2*x1-x2+0.25;
Fgrad = gradient(F, [x1,x2]);
Fhess = hessian(F,[x1,x2]);

k=1;
difference = 1;

while difference>0.000001
    g(:,k)=Fgrad(x(1,k),x(2,k));
    x(:,k+1) = x(:,k)-inv(Fhess)*g(:,k)
    difference = sqrt((x(1,k+1)-x(1,k))^2 + (x(2,k+1)-x(2,k))^2);
    k=k+1;
end

hold on
ezcontour(F,[-1 1])
title('F(x) with Newtons Method')
x=x';
for i=1:(length(x')-1)
    arrow_length = sqrt((x(i+1,1)-x(i,1))^2 + (x(i+1,2)-x(i,2))^2);
    quiver(x(i,1),x(i,2),(x(i+1,1)-x(i,1)),(x(i+1,2)-x(i,2)),1,'Color','black','MaxHeadSize',100/arrow_length);
end
hold off
%% Conjugate gradient method- part 1d
clear all
clf;

syms x1;
syms x2;
syms F(x1,x2);
initial_guess = [1 1]';
alpha = 0.01;
x(:,1) = initial_guess;
F(x1,x2) = 5*x1^2+2*x1*x2+2*x2^2-2*x1-x2+0.25;
Fgrad = gradient(F, [x1,x2]);
Fhess = hessian(F,[x1,x2]);

difference = 1;
g(:,1)=double(Fgrad(x(1,1),x(2,1)));
p(:,1) = -g(:,1);
A = double(Fhess(x(1,1),x(2,1)));
a(1) = -double(g(:,1)'*p(:,1)/(p(:,1)'*A*p(:,1)));
x(:,2)= x(:,1)+a(1)*p(:,1);

k=2;
while difference>0.000001 
    g(:,k)=double(Fgrad(x(1,k),x(2,k)));
    Beta(k) = (g(:,k)'*g(:,k))/(g(:,k-1)'*g(:,k-1));
    p(:,k) = -g(:,k)+Beta(k)*p(:,k-1);
    A = double(Fhess(x(1,k),x(2,k)));
    a(k) = -double(g(:,k)'*p(:,k)/(p(:,k)'*A*p(:,k)));
    
    x(:,k+1)= x(:,k)+a(k)*p(:,k)
    difference = sqrt((x(1,k+1)-x(1,k))^2 + (x(2,k+1)-x(2,k))^2);
    k=k+1;
end

hold on
ezcontour(F,[-1 1])
title('F(x) with Conjugate Gradient Method')
x=x';
for i=1:(length(x')-1)
    arrow_length = sqrt((x(i+1,1)-x(i,1))^2 + (x(i+1,2)-x(i,2))^2);
    quiver(x(i,1),x(i,2),(x(i+1,1)-x(i,1)),(x(i+1,2)-x(i,2)),1,'Color','black','MaxHeadSize',100/arrow_length);
end
hold off