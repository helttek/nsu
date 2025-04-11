function SolveSLAE()//1.1
    A=[-2,-1,3,2;-1,1,1,0.6667;-3,-1,-1,2;-3,-1,3,-1];
    b=[40;20;60;60];
    x=linsolve(A,(-1)*b);
    disp("SLAE solution: ");
    disp(x);
    
    bCheck=A*x;
    disp("Checking solution: ");
    disp(bCheck);
endfunction

function CalcInvert()//1.2
    A=[1,4,2;2,1,-2;0,1,-1];
    B=[4,6,-2;4,10,1;2,4,-5];
    D=2*A-(A*A+B)*B;
    if det(D)==0 then
        disp("Error: Determinant is zero.")
        return
    end
    sol=inv(D);
    disp("Inverted matrix solution: ");
    disp(sol);
    disp("Checking solution: ");
    disp(D*sol);
endfunction

function BuildFunctionPlot()//2.1
    x=-20:0.1:20;
    f=((x+5).^2).^(1/3) - ((x-7).^2).^(1/3);
    plot(x,f);
    xtitle("f", "x", "y");
    xgrid();
endfunction

function PolarPlot()//2.2
    phi = linspace(0, 2*%pi, 1000);
    for k = 1:length(phi)
        if modulo(3*phi(k), %pi) > %pi/2 - 0.01 & modulo(3*phi(k), %pi) < %pi/2 + 0.01 then
            phi(k) = %nan;
        end
    end
    rho = 2 * tan(3 * phi);
    polarplot(phi, rho);
    xtitle("Polar Plot: rho = 2 * tan(3*phi)", "phi", "rho");
    xgrid();
endfunction

function PlotXYZ()//3.1
    u=0:0.01:4*%pi;
    v=0:0.01:6*%pi;
    [U, V] = ndgrid(u, v);
    x=cos(U).*U.*(1+cos(V/2));
    y=U/2.*sin(V);
    z=sin(U).*U.*(1+cos(V/2));
    plot3d2(x,y,z);
endfunction

function PlotXYZt()//3.2
    t=0:0.01:2*%pi;
    x=sin(t);
    x1=cos(t);
    y=sin(2*t);
    y1=cos(2*t);
    z=t/5;
    z1=sin(t);
    curve=param3d(x,y,z);
    curve1=param3d(x1,y1,z1);
    curve.mark_foreground = color("magenta");
    curve.mark_mode = "on";
    curve.mark_style = 10;
    curve1.mark_foreground = color("yellow");
    curve1.mark_mode = "on";
    curve1.mark_style = 10;
endfunction

function FindRoots()//4.1
    coeffs=[3.25,7.67,5,0,-11];
    r=roots(coeffs);
    coeffs1=[2,5,11,7];
    r1=roots(coeffs1);
    disp("First roots: ");
    disp(r);
    disp("Second roots: ");
    disp(r1);
endfunction

function ExpDataTest()//5.1
    function y = funcR(c, d, k, h)
        y = c*h.^2 + d*h + k;
    endfunction

    function y = funcRDataFit(c, h)
        h = h(:);
        y = c(1).*h.^2 + c(2).*h + c(3);
    endfunction
    
    //h = [2; 4; 6; 8; 10; 12; 14; 16];
    //R = [0.035; 0.09; 0.147; 0.2; 0.24; 0.28; 0.31; 0.34];

    h = [2, 4, 6, 8,10, 12, 14, 16];
    R = [0.035, 0.09, 0.147, 0.2, 0.24, 0.28, 0.31, 0.34];
    
    X = [sum(h.^4), sum(h.^3), sum(h.^2); sum(h.^3), sum(h.^2), sum(h); sum(h.^2), sum(h), length(h)];
    Y = [sum(h.^2.*R); sum(h.*R); sum(R)];
    
    params=linsolve(X,(-1)*Y);
    
   //[params, err] = datafit(funcRDataFit, h, R, [200,1,1]);
    //mprintf("Суммарная ошибка dadafit: %.12f\n", err);
    
    C = params(1);
    D = params(2);
    K = params(3);
    
    mprintf("R = %.6f*h^2 + %.6f*h + %.6f\n", C, D, K);
    
    RPrediction = funcR(C, D, K, h);
    error = sum((R-RPrediction).^2);
    mprintf("Суммарная ошибка вручную: %.12f\n", error);

    h_fit = linspace(min(h), max(h), 100)';
    R_fit = funcR(C, D, K, h_fit);
    //R_fit = funcRDataFit(params, h_fit);
    scf(0);
    clf;
    plot(h, R, 'ob');
    plot(h_fit, R_fit, '-r');
    xlabel('h');
    ylabel('R');
    title('Dataset and Fitted Function Approximation');
    legend(['Data Points', 'Fitted Curve'], "location", "northwest");
endfunction
