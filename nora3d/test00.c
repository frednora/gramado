// TEst functions created by Copilot.

void drawStar(struct gws_window_d *window, int centerX, int centerY, int radius, int color) {
    float angle = M_PI / 5; // 36 degrees
    int x[10], y[10];
    
    // Calculate the points
    for (int i = 0; i < 10; i++) {
        float r = (i % 2 == 0) ? radius : radius / 2;
        x[i] = centerX + r * cos(i * angle - M_PI / 2);
        y[i] = centerY + r * sin(i * angle - M_PI / 2);
    }
    
    // Draw the star by connecting the points
    for (int i = 0; i < 10; i++) {
        plotLine3d(window, x[i], y[i], 0, x[(i + 2) % 10], y[(i + 2) % 10], 0, color);
    }
}

void demoStar() {
    struct gws_window_d *dw = __create_demo_window(100, 100, 400, 400);
    if ((void*) dw != NULL && dw->magic == 1234) {
        __demo_window = dw;
        drawStar(dw, 200, 200, 100, COLOR_YELLOW);
        demoFlushSurface(dw);
    }
}

void drawGrid(struct gws_window_d *window, int startX, int startY, int endX, int endY, int step, int color) {
    for (int x = startX; x <= endX; x += step) {
        plotLine3d(window, x, startY, 0, x, endY, 0, color); // vertical lines
    }
    for (int y = startY; y <= endY; y += step) {
        plotLine3d(window, startX, y, 0, endX, y, 0, color); // horizontal lines
    }
}

void demoGrid() {
    struct gws_window_d *dw = __create_demo_window(100, 100, 400, 400);
    if ((void*) dw != NULL && dw->magic == 1234) {
        __demo_window = dw;
        drawGrid(dw, 50, 50, 350, 350, 20, COLOR_WHITE);
        demoFlushSurface(dw);
    }
}

void drawHappyFace(struct gws_window_d *window, int centerX, int centerY, int radius, int color) {
    // Draw face outline
    plotCircle3d(window, centerX, centerY, radius, color);
    
    // Draw eyes
    int eyeRadius = radius / 10;
    int eyeOffsetX = radius / 3;
    int eyeOffsetY = radius / 3;
    plotCircle3d(window, centerX - eyeOffsetX, centerY - eyeOffsetY, eyeRadius, color);
    plotCircle3d(window, centerX + eyeOffsetX, centerY - eyeOffsetY, eyeRadius, color);

    // Draw mouth (a smile)
    int mouthWidth = radius / 2;
    int mouthHeight = radius / 4;
    plotArc3d(window, centerX, centerY + eyeOffsetY, mouthWidth, mouthHeight, 180, 360, color);
}

void demoHappyFace() {
    struct gws_window_d *dw = __create_demo_window(100, 100, 400, 400);
    if ((void*) dw != NULL && dw->magic == 1234) {
        __demo_window = dw;
        drawHappyFace(dw, 200, 200, 100, COLOR_YELLOW);
        demoFlushSurface(dw);
    }
}

