/**
 * ==========================================================================
 * COSMIC CANVAS ENGINE // Interstellar Space Dust, Nebulae & Twinkling Stars
 * ==========================================================================
 */
(function() {
    const canvas = document.getElementById('cosmic-canvas');
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    let width = 0;
    let height = 0;

    const STAR_COUNT = 240;
    const NEBULA_COUNT = 4;
    const stars = [];
    const nebulae = [];
    const shootingStars = [];

    function resize() {
        width = canvas.width = window.innerWidth;
        height = canvas.height = window.innerHeight;
    }

    window.addEventListener('resize', resize);
    resize();

    // Initialize Stars
    const starColors = ['#ffffff', '#00f2fe', '#4facfe', '#bd00ff', '#ffeaa7', '#ff7675'];
    for (let i = 0; i < STAR_COUNT; i++) {
        stars.push({
            x: Math.random() * width,
            y: Math.random() * height,
            radius: Math.random() * 1.6 + 0.3,
            color: starColors[Math.floor(Math.random() * starColors.length)],
            alpha: Math.random() * 0.8 + 0.2,
            twinkleSpeed: (Math.random() * 0.02 + 0.005) * (Math.random() > 0.5 ? 1 : -1),
            vx: (Math.random() - 0.5) * 0.1,
            vy: (Math.random() - 0.5) * 0.1
        });
    }

    // Initialize Nebulae Cloud Centers
    const nebulaColors = [
        { r: 0, g: 242, b: 254, a: 0.045 }, // Cyan
        { r: 189, g: 0, b: 255, a: 0.04 },  // Violet
        { r: 0, g: 80, b: 200, a: 0.06 },   // Deep Blue
        { r: 255, g: 0, b: 128, a: 0.03 }   // Magenta
    ];

    for (let i = 0; i < NEBULA_COUNT; i++) {
        nebulae.push({
            x: Math.random() * width,
            y: Math.random() * height,
            radius: Math.random() * 350 + 250,
            color: nebulaColors[i % nebulaColors.length],
            angle: Math.random() * Math.PI * 2,
            speed: (Math.random() * 0.0008 + 0.0003)
        });
    }

    // Spawn shooting stars periodically
    function maybeSpawnShootingStar() {
        if (Math.random() < 0.012 && shootingStars.length < 2) {
            shootingStars.push({
                x: Math.random() * width,
                y: Math.random() * (height * 0.4),
                length: Math.random() * 80 + 40,
                speed: Math.random() * 8 + 6,
                angle: Math.PI / 4 + (Math.random() - 0.5) * 0.2,
                opacity: 1,
                decay: 0.025
            });
        }
    }

    function render() {
        ctx.clearRect(0, 0, width, height);

        // 1. Render Nebular Clouds
        for (const neb of nebulae) {
            neb.angle += neb.speed;
            const curX = neb.x + Math.cos(neb.angle) * 40;
            const curY = neb.y + Math.sin(neb.angle) * 40;

            const grad = ctx.createRadialGradient(curX, curY, 0, curX, curY, neb.radius);
            const { r, g, b, a } = neb.color;
            grad.addColorStop(0, `rgba(${r}, ${g}, ${b}, ${a * 1.5})`);
            grad.addColorStop(0.5, `rgba(${r}, ${g}, ${b}, ${a})`);
            grad.addColorStop(1, 'rgba(2, 4, 10, 0)');

            ctx.fillStyle = grad;
            ctx.beginPath();
            ctx.arc(curX, curY, neb.radius, 0, Math.PI * 2);
            ctx.fill();
        }

        // 2. Render Twinkling Stars
        for (const star of stars) {
            star.alpha += star.twinkleSpeed;
            if (star.alpha > 1 || star.alpha < 0.15) {
                star.twinkleSpeed = -star.twinkleSpeed;
            }

            star.x += star.vx;
            star.y += star.vy;
            if (star.x < 0) star.x = width;
            if (star.x > width) star.x = 0;
            if (star.y < 0) star.y = height;
            if (star.y > height) star.y = 0;

            ctx.save();
            ctx.globalAlpha = star.alpha;
            ctx.fillStyle = star.color;
            ctx.beginPath();
            ctx.arc(star.x, star.y, star.radius, 0, Math.PI * 2);
            ctx.fill();

            // Glow around brightest stars
            if (star.radius > 1.3) {
                ctx.shadowColor = star.color;
                ctx.shadowBlur = 8;
                ctx.beginPath();
                ctx.arc(star.x, star.y, star.radius * 0.8, 0, Math.PI * 2);
                ctx.fill();
            }
            ctx.restore();
        }

        // 3. Render Shooting Stars
        maybeSpawnShootingStar();
        for (let i = shootingStars.length - 1; i >= 0; i--) {
            const ss = shootingStars[i];
            ss.x += Math.cos(ss.angle) * ss.speed;
            ss.y += Math.sin(ss.angle) * ss.speed;
            ss.opacity -= ss.decay;

            if (ss.opacity <= 0 || ss.x > width || ss.y > height) {
                shootingStars.splice(i, 1);
                continue;
            }

            ctx.save();
            ctx.globalAlpha = ss.opacity;
            const tailX = ss.x - Math.cos(ss.angle) * ss.length;
            const tailY = ss.y - Math.sin(ss.angle) * ss.length;

            const grad = ctx.createLinearGradient(tailX, tailY, ss.x, ss.y);
            grad.addColorStop(0, 'rgba(0, 242, 254, 0)');
            grad.addColorStop(1, '#ffffff');

            ctx.strokeStyle = grad;
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(tailX, tailY);
            ctx.lineTo(ss.x, ss.y);
            ctx.stroke();
            ctx.restore();
        }

        requestAnimationFrame(render);
    }

    render();
})();

