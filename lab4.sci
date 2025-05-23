function PI()
    clf();
    T = 0;
    n = 4;
    T0 = 1.23;
    
    K = 0.6375;
    Ti = 5.25;

    S = poly(0, 's');
    W0 = 2 / (1 + S * T0)^n;
    Wl = (1 + 1 / (Ti * S)) * K * W0;
    W = Wl / (1 + Wl);
    disp("W:");
    disp(W);

    data = fscanfMat("C:\otu\lab4_cir.TNO");
    y_ref = data(:, 2);

    stepRef = data(2, 1) - data(1, 1);

    densities_to_test = [0.1, 1, 10, 100, 1000, 10000];
    num_simulations = length(densities_to_test);
    simulation_duration = 100;

    figure(0);
    for k = 1:num_simulations
        current_density = densities_to_test(k);
        timeStep = 1 / current_density;
        disp("Current Density = " + string(current_density));
        sl = syslin('c', W);
        dMat = dscr(sl, timeStep);
    
        timePoints = [0:timeStep:simulation_duration];

        num_steps = length(timePoints);
        stateVector = zeros(size(dMat.A, 1), 1);
        output = zeros(timePoints); //x(t)

        for i = 1:num_steps
            stateVector = dMat.A * stateVector + dMat.B;
            output(i) = dMat.C * stateVector;
        end

        subplot(num_simulations, 1, k);
        plot(timePoints, output, 'blue');
        plot(data(:, 1), data(:, 2), 'red');

        //calc err
        err_sum_sq = 0;
        num_points_for_err = length(output);
        for l = 1:num_points_for_err
            idx = round(timePoints(l) / (stepRef + %eps)) + 1;
            idx = min(max(idx, 1), length(y_ref));
            err_sum_sq = err_sum_sq + (y_ref(idx) - output(l))^2;
        end
        calculated_rmse = sqrt(err_sum_sq / num_points_for_err);
        disp(calculated_rmse);
    end
endfunction

function PID()
    clf();
    T = 0;
    n = 4;
    T0 = 1.23;
    
    K = 1;
    Ti = 4.9;
    Td = Ti / 4;
    Tc = Td / 8;

    S = poly(0, 's');
    W0 = 2 / (1 + S * T0)^n;
    Wl = (1 + 1 / (Ti * S) + ((Td * S) / (1 + Tc * S)) ) * K * W0;
    W = Wl / (1 + Wl);
    disp("W:");
    disp(W);

    data = fscanfMat("C:\otu\lab4_PID.TNO");
    y_ref = data(:, 2);

    stepRef = data(2, 1) - data(1, 1);

    densities_to_test = [0.1, 1, 10, 100, 1000, 10000];
    num_simulations = length(densities_to_test);
    simulation_duration = 100;

    figure(0);
    for k = 1:num_simulations
        current_density = densities_to_test(k);
        timeStep = 1 / current_density;
        disp("Current Density = " + string(current_density));
        sl = syslin('c', W);
        dMat = dscr(sl, timeStep);
    
        timePoints = [0:timeStep:simulation_duration];

        num_steps = length(timePoints);
        stateVector = zeros(size(dMat.A, 1), 1);
        output = zeros(timePoints);

        for i = 1:num_steps
            stateVector = dMat.A * stateVector + dMat.B;
            output(i) = dMat.C * stateVector;
        end

        subplot(num_simulations, 1, k);
        plot(timePoints, output, 'blue');
        plot(data(:, 1), data(:, 2), 'red');

        err_sum_sq = 0;
        num_points_for_err = length(output);
        for l = 1:num_points_for_err
            idx = round(timePoints(l) / (stepRef + %eps)) + 1;
            idx = min(max(idx, 1), length(y_ref));

            err_sum_sq = err_sum_sq + (y_ref(idx) - output(l))^2;
        end
        calculated_rmse = sqrt(err_sum_sq / num_points_for_err);
        disp(calculated_rmse);

    end
endfunction
