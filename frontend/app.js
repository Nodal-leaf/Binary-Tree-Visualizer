/**
 * ==========================================================================
 * CHRONO-COSMOS // APPLICATION CONTROLLER & AUDIO SYNTHESIS
 * ==========================================================================
 */

// ==========================================================================
// 1. QUANTUM AUDIO SYNTHESIS ENGINE (Web Audio API)
// ==========================================================================
class QuantumAudio {
    constructor() {
        this.ctx = null;
        this.enabled = true;
    }

    init() {
        if (!this.ctx && typeof AudioContext !== 'undefined') {
            this.ctx = new (window.AudioContext || window.webkitAudioContext)();
        }
        if (this.ctx && this.ctx.state === 'suspended') {
            this.ctx.resume();
        }
    }

    playNodeHover(val) {
        if (!this.enabled) return;
        this.init();
        if (!this.ctx) return;

        const osc = this.ctx.createOscillator();
        const gain = this.ctx.createGain();

        // Frequency mapped harmonically from star key
        const freq = 260 + (val % 700) * 0.8;
        osc.type = 'sine';
        osc.frequency.setValueAtTime(freq, this.ctx.currentTime);

        gain.gain.setValueAtTime(0.12, this.ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.28);

        osc.connect(gain);
        gain.connect(this.ctx.destination);

        osc.start();
        osc.stop(this.ctx.currentTime + 0.28);
    }

    playWarp() {
        if (!this.enabled) return;
        this.init();
        if (!this.ctx) return;

        // Gravitational frequency sweep
        const osc = this.ctx.createOscillator();
        const gain = this.ctx.createGain();

        osc.type = 'sawtooth';
        osc.frequency.setValueAtTime(180, this.ctx.currentTime);
        osc.frequency.exponentialRampToValueAtTime(880, this.ctx.currentTime + 0.5);

        gain.gain.setValueAtTime(0.15, this.ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.6);

        osc.connect(gain);
        gain.connect(this.ctx.destination);

        osc.start();
        osc.stop(this.ctx.currentTime + 0.6);
    }

    playSuccess() {
        if (!this.enabled) return;
        this.init();
        if (!this.ctx) return;

        const notes = [523.25, 659.25, 783.99, 1046.50]; // C E G C chord
        notes.forEach((freq, i) => {
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();

            osc.type = 'triangle';
            osc.frequency.setValueAtTime(freq, this.ctx.currentTime + i * 0.08);

            gain.gain.setValueAtTime(0.1, this.ctx.currentTime + i * 0.08);
            gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + i * 0.08 + 0.4);

            osc.connect(gain);
            gain.connect(this.ctx.destination);

            osc.start(this.ctx.currentTime + i * 0.08);
            osc.stop(this.ctx.currentTime + i * 0.08 + 0.4);
        });
    }

    playError() {
        if (!this.enabled) return;
        this.init();
        if (!this.ctx) return;

        const osc = this.ctx.createOscillator();
        const gain = this.ctx.createGain();

        osc.type = 'sawtooth';
        osc.frequency.setValueAtTime(140, this.ctx.currentTime);
        osc.frequency.linearRampToValueAtTime(90, this.ctx.currentTime + 0.25);

        gain.gain.setValueAtTime(0.18, this.ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, this.ctx.currentTime + 0.28);

        osc.connect(gain);
        gain.connect(this.ctx.destination);

        osc.start();
        osc.stop(this.ctx.currentTime + 0.28);
    }
}

window.AudioEngine = new QuantumAudio();

// ==========================================================================
// 2. MAIN APPLICATION CORE
// ==========================================================================
class CosmosApp {
    constructor() {
        // API Base URL Detection
        this.apiBase = (window.location.protocol.startsWith('http')) 
            ? `${window.location.origin}/api` 
            : 'http://127.0.0.1:8080/api';

        this.currentMode = 'AVL';
        this.speedMultiplier = 1.0;
        this.isBusy = false;

        this.visualizer = new TreeVisualizer('tree-svg', {
            onNodeClick: (node) => this.showNodeTelemetry(node)
        });

        this.initElements();
        this.bindEvents();
        this.connectBackend();
    }

    initElements() {
        // Inputs & buttons
        this.inputVal = document.getElementById('node-value-input');
        this.btnRandom = document.getElementById('btn-random-val');
        this.btnInsert = document.getElementById('btn-insert');
        this.btnDelete = document.getElementById('btn-delete');
        this.btnSearch = document.getElementById('btn-search');
        this.btnSample = document.getElementById('btn-sample');
        this.btnClear = document.getElementById('btn-clear');

        // Mode switchers
        this.modeBst = document.getElementById('mode-bst');
        this.modeAvl = document.getElementById('mode-avl');

        // View controls
        this.btnZoomIn = document.getElementById('btn-zoom-in');
        this.btnZoomOut = document.getElementById('btn-zoom-out');
        this.btnZoomReset = document.getElementById('btn-zoom-reset');

        // Telemetry stats
        this.statNodes = document.getElementById('stat-nodes');
        this.statHeight = document.getElementById('stat-height');
        this.statStatus = document.getElementById('stat-status');

        // Panels
        this.nodeCard = document.getElementById('selected-node-card');
        this.missionLog = document.getElementById('mission-log');
        this.btnClearLog = document.getElementById('btn-clear-log');
        this.warpBanner = document.getElementById('warp-banner');
        this.warpTitle = document.getElementById('warp-title');
        this.warpDesc = document.getElementById('warp-desc');

        // Traversal UI
        this.traversalDisplay = document.getElementById('traversal-display');
        this.traversalTitle = document.getElementById('traversal-title');
        this.traversalChips = document.getElementById('traversal-chips');
        this.btnCloseTraversal = document.getElementById('btn-close-traversal');

        // Speed & Audio
        this.speedSlider = document.getElementById('speed-slider');
        this.speedDisplay = document.getElementById('speed-display');
        this.audioToggle = document.getElementById('audio-toggle');
    }

    bindEvents() {
        // User gesture unlocks Web Audio
        window.addEventListener('click', () => window.AudioEngine.init(), { once: true });

        // Action Buttons
        this.btnInsert.addEventListener('click', () => this.handleInsert());
        this.btnDelete.addEventListener('click', () => this.handleDelete());
        this.btnSearch.addEventListener('click', () => this.handleSearch());
        this.btnSample.addEventListener('click', () => this.handleSample());
        this.btnClear.addEventListener('click', () => this.handleClear());
        this.btnRandom.addEventListener('click', () => this.handleRandomValue());

        // Enter key in input triggers insert
        this.inputVal.addEventListener('keydown', (e) => {
            if (e.key === 'Enter') {
                this.handleInsert();
            }
        });

        // Mode switch
        this.modeBst.addEventListener('click', () => this.setMode('BST'));
        this.modeAvl.addEventListener('click', () => this.setMode('AVL'));

        // Viewport controls
        this.btnZoomIn.addEventListener('click', () => this.visualizer.zoomIn());
        this.btnZoomOut.addEventListener('click', () => this.visualizer.zoomOut());
        this.btnZoomReset.addEventListener('click', () => this.visualizer.resetView());

        // Traversal Buttons
        document.querySelectorAll('.traversal-btn').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const type = btn.getAttribute('data-traversal');
                this.handleTraversal(type);
            });
        });

        this.btnCloseTraversal.addEventListener('click', () => {
            this.traversalDisplay.classList.add('hidden');
        });

        // Speed Slider
        this.speedSlider.addEventListener('input', (e) => {
            this.speedMultiplier = parseFloat(e.target.value);
            this.speedDisplay.textContent = `${this.speedMultiplier.toFixed(1)}x`;
        });

        // Audio Toggle
        this.audioToggle.addEventListener('change', (e) => {
            window.AudioEngine.enabled = e.target.checked;
        });

        // Clear Log
        this.btnClearLog.addEventListener('click', () => {
            this.missionLog.innerHTML = '';
        });
    }

    log(type, message) {
        const entry = document.createElement('div');
        entry.className = `log-entry ${type}`;

        const time = new Date().toLocaleTimeString([], { hour12: false });
        entry.innerHTML = `<span class="log-time">[${time}]</span><span class="log-text">${message}</span>`;

        this.missionLog.appendChild(entry);
        this.missionLog.scrollTop = this.missionLog.scrollHeight;
    }

    setBusy(busy) {
        this.isBusy = busy;
        const buttons = [
            this.btnInsert, this.btnDelete, this.btnSearch, 
            this.btnSample, this.btnClear, this.modeBst, this.modeAvl
        ];
        buttons.forEach(b => {
            if (b) b.disabled = busy;
        });
    }

    updateTelemetry(treeData) {
        if (!treeData) return;
        this.statNodes.textContent = treeData.nodeCount || 0;
        this.statHeight.textContent = treeData.height || 0;

        if (treeData.treeType) {
            this.currentMode = treeData.treeType;
            if (this.currentMode === 'BST') {
                this.modeBst.classList.add('active');
                this.modeAvl.classList.remove('active');
            } else {
                this.modeAvl.classList.add('active');
                this.modeBst.classList.remove('active');
            }
        }
    }

    showWarpBanner(rotations) {
        if (!rotations || rotations.length === 0) return;

        const rot = rotations[rotations.length - 1]; // latest rotation
        this.warpTitle.textContent = `${rot.type} ROTATION`;
        this.warpDesc.textContent = rot.description;
        this.warpBanner.classList.remove('hidden');

        this.log('warp', `[GRAVITATIONAL WARP] ${rot.description}`);

        setTimeout(() => {
            this.warpBanner.classList.add('hidden');
        }, 3200);
    }

    showNodeTelemetry(node) {
        if (!node || !this.nodeCard) return;
        const bf = node.balanceFactor || 0;
        let bfLabel = 'Balanced (0)';
        if (Math.abs(bf) === 1) bfLabel = `Tilted (${bf > 0 ? '+' : ''}${bf})`;
        if (Math.abs(bf) > 1) bfLabel = `Critical (${bf > 0 ? '+' : ''}${bf})`;

        this.nodeCard.classList.remove('hidden');
        this.nodeCard.innerHTML = `
            <div class="node-card-content">
                <span class="node-badge">NODE <b>${node.value}</b></span>
                <span class="node-meta">H: <b>${node.height}</b></span>
                <span class="node-meta">BF: <b>${bfLabel}</b></span>
                <button id="quick-sweep" class="btn btn-sm">Search</button>
                <button id="quick-collapse" class="btn btn-sm btn-danger">Delete</button>
                <button id="quick-close" class="close-btn" style="margin-left:4px">✕</button>
            </div>
        `;

        document.getElementById('quick-sweep')?.addEventListener('click', () => {
            this.inputVal.value = node.value;
            this.handleSearch();
        });

        document.getElementById('quick-collapse')?.addEventListener('click', () => {
            this.inputVal.value = node.value;
            this.handleDelete();
        });

        document.getElementById('quick-close')?.addEventListener('click', () => {
            this.nodeCard.classList.add('hidden');
            this.visualizer.clearHighlights();
        });
    }

    // ==========================================================================
    // BACKEND API CALLS
    // ==========================================================================
    async connectBackend() {
        this.log('info', `Connecting to C++ Quantum Core at ${this.apiBase}...`);
        try {
            const res = await fetch(`${this.apiBase}/tree`);
            if (res.ok) {
                const data = await res.json();
                if (this.statStatus) {
                    this.statStatus.className = 'status-dot online';
                    this.statStatus.title = 'Core Online';
                }
                this.log('success', 'Connected to C++ Backend REST Engine.');
                this.updateTelemetry(data.tree);
                this.visualizer.renderTree(data.tree);
            } else {
                throw new Error('Non-200 response');
            }
        } catch (err) {
            if (this.statStatus) {
                this.statStatus.className = 'status-dot offline';
                this.statStatus.title = 'Offline (Launch server)';
            }
            this.log('warn', 'C++ backend offline. Launch build/tree_server.exe to sync with C++ server.');
        }
    }

    getInputValue() {
        const val = parseInt(this.inputVal.value.trim(), 10);
        if (isNaN(val) || val <= 0 || val > 9999) {
            this.log('error', 'Please enter a valid positive star energy key (1 - 9999).');
            this.inputVal.focus();
            return null;
        }
        return val;
    }

    async handleInsert() {
        const val = this.getInputValue();
        if (val === null || this.isBusy) return;

        this.setBusy(true);
        this.log('info', `Spawning celestial entity with key ${val}...`);

        try {
            const res = await fetch(`${this.apiBase}/insert`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ value: val })
            });

            const data = await res.json();
            if (data.success) {
                // Step 1: Trace insertion comparison sweep
                if (data.searchPath && data.searchPath.length > 0) {
                    await this.visualizer.animateSearchPath(data.searchPath, true, this.speedMultiplier);
                }

                // Step 2: Render newly inserted star and updated layout
                this.visualizer.renderTree(data.tree);
                this.updateTelemetry(data.tree);

                // Step 3: If AVL rebalancing occurred, animate each rotation with flux arcs
                if (data.tree && data.tree.lastRotations && data.tree.lastRotations.length > 0) {
                    this.showWarpBanner(data.tree.lastRotations);
                    for (const rot of data.tree.lastRotations) {
                        await this.visualizer.animateRotation(rot, this.speedMultiplier);
                    }
                }
                this.log('success', data.message);
                this.inputVal.value = '';
            } else {
                this.log('warn', data.message);
                if (window.AudioEngine) window.AudioEngine.playError();
            }
        } catch (err) {
            this.log('error', `Server communication failed: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    async handleDelete() {
        const val = this.getInputValue();
        if (val === null || this.isBusy) return;

        this.setBusy(true);
        this.log('info', `Collapsing star with key ${val}...`);

        try {
            const res = await fetch(`${this.apiBase}/delete`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ value: val })
            });

            const data = await res.json();
            if (data.success) {
                // Step 1: Animate search sweep to target node
                if (data.searchPath && data.searchPath.length > 0) {
                    await this.visualizer.animateSearchPath(data.searchPath, true, this.speedMultiplier);
                }

                // Step 2: Check if two-children deletion swap occurred
                const deletionInfo = data.lastDeletion || (data.tree && data.tree.lastDeletion);
                if (deletionInfo && deletionInfo.hasTwoChildren && deletionInfo.successorValue) {
                    this.log('warn', `[STELLAR SWAP] Target Star ${deletionInfo.targetValue} has 2 children -> Swapping with Inorder Successor Star ${deletionInfo.successorValue}`);
                    await this.visualizer.animateSwap(deletionInfo.targetValue, deletionInfo.successorValue, this.speedMultiplier);
                }

                // Step 3: Render updated tree state
                this.visualizer.renderTree(data.tree);
                this.updateTelemetry(data.tree);

                // Step 4: Animate any AVL rotations triggered by the deletion rebalancing
                if (data.tree && data.tree.lastRotations && data.tree.lastRotations.length > 0) {
                    this.showWarpBanner(data.tree.lastRotations);
                    for (const rot of data.tree.lastRotations) {
                        await this.visualizer.animateRotation(rot, this.speedMultiplier);
                    }
                }

                this.log('success', data.message);
                this.inputVal.value = '';
            } else {
                this.log('warn', data.message);
                if (window.AudioEngine) window.AudioEngine.playError();
            }
        } catch (err) {
            this.log('error', `Server communication failed: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    async handleSearch() {
        const val = this.getInputValue();
        if (val === null || this.isBusy) return;

        this.setBusy(true);
        this.log('info', `Sweeping constellation sensor for star ${val}...`);

        try {
            const res = await fetch(`${this.apiBase}/search`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ value: val })
            });

            const data = await res.json();
            if (data.path && data.path.length > 0) {
                await this.visualizer.animateSearchPath(data.path, data.found, this.speedMultiplier);
            }

            if (data.found) {
                this.log('success', `[SENSOR HIT] Star ${val} located in constellation!`);
            } else {
                this.log('warn', `[SENSOR MISS] Star ${val} not detected in orbital range.`);
            }
        } catch (err) {
            this.log('error', `Server communication failed: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    async setMode(mode) {
        if (this.currentMode === mode || this.isBusy) return;
        this.setBusy(true);
        this.log('info', `Shifting gravitational architecture to ${mode}...`);

        try {
            const res = await fetch(`${this.apiBase}/tree/type`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ type: mode, preserve: true })
            });

            const data = await res.json();
            if (data.success) {
                this.currentMode = mode;
                this.visualizer.renderTree(data.tree);
                this.updateTelemetry(data.tree);
                this.log('success', `Gravitational architecture stabilized in ${mode} mode.`);
                if (window.AudioEngine) window.AudioEngine.playWarp();
            }
        } catch (err) {
            this.log('error', `Failed to switch mode: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    async handleSample() {
        if (this.isBusy) return;
        this.setBusy(true);
        this.log('info', 'Loading harmonic preset constellation...');

        try {
            const res = await fetch(`${this.apiBase}/sample`, { method: 'POST' });
            const data = await res.json();
            if (data.success) {
                this.visualizer.renderTree(data.tree);
                this.updateTelemetry(data.tree);
                this.log('success', 'Harmonic constellation loaded successfully.');
                if (window.AudioEngine) window.AudioEngine.playSuccess();
            }
        } catch (err) {
            this.log('error', `Failed to load preset: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    async handleClear() {
        if (this.isBusy) return;
        this.setBusy(true);
        this.log('warn', 'Initiating Supernova Collapse (clearing all entities)...');

        try {
            const res = await fetch(`${this.apiBase}/clear`, { method: 'POST' });
            const data = await res.json();
            if (data.success) {
                this.visualizer.renderTree(data.tree);
                this.updateTelemetry(data.tree);
                this.log('info', 'All star entities collapsed into the quantum void.');
                if (window.AudioEngine) window.AudioEngine.playWarp();
            }
        } catch (err) {
            this.log('error', `Clear failed: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    async handleTraversal(type) {
        if (this.isBusy) return;
        this.setBusy(true);
        this.log('info', `Computing ${type.toUpperCase()} constellation sequence...`);

        try {
            const res = await fetch(`${this.apiBase}/traversal/${type}`);
            const data = await res.json();

            if (data.success && data.sequence && data.sequence.length > 0) {
                this.traversalTitle.textContent = `${type.toUpperCase()} BEACON SEQUENCE (${data.sequence.length} STARS)`;
                this.traversalChips.innerHTML = '';
                this.traversalDisplay.classList.remove('hidden');

                // Render chips
                const chipEls = [];
                data.sequence.forEach((val, i) => {
                    const chip = document.createElement('span');
                    chip.className = 'star-chip';
                    chip.textContent = val;
                    this.traversalChips.appendChild(chip);
                    chipEls.push(chip);
                });

                // Animate traversal with step highlight
                await this.visualizer.animateTraversal(data.sequence, this.speedMultiplier, (val, index) => {
                    chipEls.forEach((c, idx) => {
                        if (idx === index) {
                            c.classList.add('active');
                            c.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
                        }
                    });
                });

                this.log('success', `${type.toUpperCase()} traversal completed.`);
            } else {
                this.log('warn', 'Constellation is empty. Cannot traverse.');
            }
        } catch (err) {
            this.log('error', `Traversal failed: ${err.message}`);
        } finally {
            this.setBusy(false);
        }
    }

    handleRandomValue() {
        const randomVal = Math.floor(Math.random() * 95) + 5;
        this.inputVal.value = randomVal;
        this.log('info', `Sensor acquired random energy reading: ${randomVal}`);
    }
}

// Instantiate on DOM load
window.addEventListener('DOMContentLoaded', () => {
    window.app = new CosmosApp();
});

