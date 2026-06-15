function Kalman_filter_response()
    % KALMAN_FILTER_RESPONSE
    % A real-time, interactive MATLAB GUI for the HX711 Kalman Filter.

    %% 1. Create the Main UI Figure
    fig = uifigure('Name', 'Interactive Kalman Filter', 'Position', [100 100 1000 650], 'Color', 'w');

    %% 2. Create the Axes for Plotting
    ax = uiaxes(fig, 'Position', [50 180 900 420]);
    title(ax, 'Real-Time Kalman Filter Response', 'FontSize', 14);
    xlabel(ax, 'Time Window (Scrolling)');
    ylabel(ax, 'Weight (grams)');
    grid(ax, 'on');
    hold(ax, 'on');

    %% 3. Create UI Controls (Sliders and Buttons)
    
    % Q Slider (Process Noise) - Logarithmic Scale (-6 to 0)
    uilabel(fig, 'Position', [50 100 150 22], 'Text', 'Process Noise (Q) Log10:', 'FontWeight', 'bold');
    q_sld = uislider(fig, 'Position', [220 110 200 3], 'Limits', [-6, 0], 'Value', -4);
    q_val_lbl = uilabel(fig, 'Position', [430 100 100 22], 'Text', '1.0e-04');

    % R Slider (Measurement Noise) - Logarithmic Scale (-3 to 1)
    uilabel(fig, 'Position', [50 50 150 22], 'Text', 'Meas. Noise (R) Log10:', 'FontWeight', 'bold');
    r_sld = uislider(fig, 'Position', [220 60 200 3], 'Limits', [-3, 1], 'Value', -2);
    r_val_lbl = uilabel(fig, 'Position', [430 50 100 22], 'Text', '1.0e-02');

    % Random Weight Shift Button
    shift_btn = uibutton(fig, 'Position', [550 70 160 40], 'Text', 'Apply Random Weight Shift', ...
                         'BackgroundColor', [0.2 0.6 0.8], 'FontColor', 'w', 'FontWeight', 'bold');
    
    % Event flag for the button
    shift_flag = false;
    shift_btn.ButtonPushedFcn = @(~, ~) trigger_shift();

    %% 4. Create Real-Time Info Display Panel
    info_pnl = uipanel(fig, 'Position', [750 40 200 100], 'Title', 'Live Telemetry', 'BackgroundColor', 'w');
    info_lbl = uilabel(info_pnl, 'Position', [10 10 180 60], 'Text', 'Initializing...', 'VerticalAlignment', 'top');

    %% 5. Initialize Simulation Variables
    num_display = 150; % Number of points to show on screen at once (scrolling window)
    
    % Pre-allocate arrays with NaNs for a clean scrolling effect
    true_w_hist = nan(1, num_display);
    z_hist = nan(1, num_display);
    x_est_hist = nan(1, num_display);

    % Initial Kalman States
    x_est = 0;
    P_est = 1;
    current_weight = 1000.0; % Start at 1 kg

    % Setup Plot Lines
    h_raw = plot(ax, z_hist, '.', 'Color', [0.7 0.7 0.7], 'DisplayName', 'Noisy Sensor (z_k)');
    h_true = plot(ax, true_w_hist, 'r--', 'LineWidth', 1.5, 'DisplayName', 'True Weight');
    h_filt = plot(ax, x_est_hist, 'b', 'LineWidth', 2.5, 'DisplayName', 'Kalman Estimate');
    legend(ax, 'Location', 'northeast');

    %% 6. Real-Time Execution Loop
    % Loop runs continuously as long as the window remains open
    while isvalid(fig)
        
        % Read interactive parameters from sliders
        Q = 10^(q_sld.Value);
        R = 10^(r_sld.Value);
        
        % Update UI labels to show exact scientific notation
        q_val_lbl.Text = sprintf('%.1e', Q);
        r_val_lbl.Text = sprintf('%.1e', R);

        % Handle manual shift if the button was pressed
        if shift_flag
            % Shift the true weight by a random amount between -500g and +500g
            current_weight = current_weight + (rand() * 1000 - 500);
            shift_flag = false;
        end

        % --- Data Generation ---
        % Shift histories left to create scrolling effect
        true_w_hist(1:end-1) = true_w_hist(2:end);
        true_w_hist(end) = current_weight;
        
        % Generate noisy reading based on the current R slider value
        z_val = current_weight + sqrt(R) * randn(); 
        z_hist(1:end-1) = z_hist(2:end);
        z_hist(end) = z_val;

        % --- Kalman Filter Equations ---
        x_pred = x_est;
        P_pred = P_est + Q;
        
        K_gain = P_pred / (P_pred + R);
        
        x_est = x_pred + K_gain * (z_val - x_pred);
        P_est = (1 - K_gain) * P_pred;

        x_est_hist(1:end-1) = x_est_hist(2:end);
        x_est_hist(end) = x_est;

        % --- Update Visuals ---
        h_raw.YData = z_hist;
        h_true.YData = true_w_hist;
        h_filt.YData = x_est_hist;

        % Dynamically adjust Y-axis so data stays centered
        valid_z = z_hist(~isnan(z_hist));
        if ~isempty(valid_z)
            y_min = min(valid_z);
            y_max = max(valid_z);
            % Add padding only if the range is greater than 0
            if y_max > y_min
                ylim(ax, [y_min - 20, y_max + 20]);
            end
        end

        % Update the Telemetry Text Box
        info_lbl.Text = sprintf('True Wt: %7.1f g\nSensor:  %7.1f g\nEstimate: %7.1f g', ...
            current_weight, z_val, x_est);

        % Render the graphics immediately without blocking
        drawnow limitrate;
        
        % Control the speed (approx. 20 FPS). Lowering this number makes it run faster.
        pause(0.05); 
    end

    function trigger_shift()
        shift_flag = true;
    end
end
