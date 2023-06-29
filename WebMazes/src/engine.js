Number.prototype.clamp = function(min, max) { return Math.min(Math.max(this, min), max); };

class GameEngine {
    constructor(appName, screenWidth, screenHeight) {
        this.canvas = document.querySelector("div#main canvas#screen");
        this.title = document.querySelector("div#main div#info h1#title");
        this.fps = document.querySelector("div#main div#info h1#fps");

        this.setTitle(appName);
        this.resizeScreen(screenWidth, screenHeight);

        this.ctx = this.canvas.getContext("2d");
        this.mouse = { x: 0, y: 0 };

        document.addEventListener("mousemove", (evt) => {
            const rect = this.canvas.getBoundingClientRect();
            this.mouse.x = (((evt.clientX - rect.left) / (rect.right - rect.left)) * this.width()).clamp(0, this.width());
            this.mouse.y = (((evt.clientY - rect.top) / (rect.bottom - rect.top)) * this.height()).clamp(0, this.height());
        });
    }

    mainLoop() {
        if (!this.onCreate()) return;

        let t1 = Date.now();
        let t2 = t1;

        let tickTimer = 0.0;

        const frameCallback = () => {
            const dt = (t2 - t1) / 1000.0;
            t1 = t2;
            t2 = Date.now();

            if (this.onUpdate(dt)) {
                tickTimer += dt;

                if (tickTimer >= 1.0) {
                    this._updateFps(Math.floor(1.0 / dt));
                    tickTimer = 0.0;
                }
                
                window.requestAnimationFrame(frameCallback);
            }
        };

        window.requestAnimationFrame(frameCallback);
    }

    _updateFps(newFps) { this.fps.innerHTML = "FPS: " + newFps; }

    setTitle(title) { this.title.innerHTML = title; }

    // WARNING! Canvas resets to the origin state after calling this method
    resizeScreen(width, height) {
        this.canvas.width = width;
        this.canvas.height = height;
    }
    
    width() { return this.canvas.width; }
    height() { return this.canvas.height; }

    screenSize() {
        return {
            width: this.width(),
            height: this.height()
        }
    }

    fillRect(x, y, w, h, col) {
        this.ctx.fillStyle = col;
        this.ctx.fillRect(x, y, w, h);
    }

    clear(col) { this.fillRect(0, 0, this.width(), this.height(), col); }

    drawRect(x, y, w, h, col) {
        this.ctx.strokeStyle = col;
        this.ctx.strokeRect(x, y, w, h);
    }

    drawLine(x1, y1, x2, y2, col, width=1) {
        this.ctx.strokeStyle = col;
        this.ctx.beginPath();
        this.ctx.moveTo(x1, y1);
        this.ctx.lineTo(x2, y2);
        this.ctx.lineWidth = width;
        this.ctx.stroke();
    }

}