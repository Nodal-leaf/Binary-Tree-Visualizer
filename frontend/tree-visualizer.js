/**
 * ==========================================================================
 * TREE VISUALIZER ENGINE // SVG Rendering, Dynamic Layout & Cosmic Animation
 * ==========================================================================
 */

class TreeVisualizer {
    constructor(svgElementId, options = {}) {
        this.svg = document.getElementById(svgElementId);
        this.viewportGroup = document.getElementById('viewport-group');
        this.edgesLayer = document.getElementById('edges-layer');
        this.nodesLayer = document.getElementById('nodes-layer');
        this.particlesLayer = document.getElementById('particles-layer');

        this.onNodeClick = options.onNodeClick || null;

        // Viewport transform (Pan & Zoom)
        this.scale = 1.0;
        this.panX = 0;
        this.panY = 0;
        this.isDragging = false;
        this.startX = 0;
        this.startY = 0;

        // Tree dimensions and node cache
        this.currentTree = null;
        this.nodeCoordinates = new Map(); // value -> {x, y, data, id}
        this.nodeElements = new Map();    // value -> SVG group element
        this.edgeElements = new Map();    // "parentVal-childVal" -> SVG line element

        this.initPanZoom();
    }

    initPanZoom() {
        const container = this.svg.parentElement;

        container.addEventListener('mousedown', (e) => {
            // Don't drag if clicking directly on a node
            if (e.target.closest('.star-group')) return;
            this.isDragging = true;
            this.startX = e.clientX - this.panX;
            this.startY = e.clientY - this.panY;
        });

        window.addEventListener('mousemove', (e) => {
            if (!this.isDragging) return;
            this.panX = e.clientX - this.startX;
            this.panY = e.clientY - this.startY;
            this.updateTransform();
        });

        window.addEventListener('mouseup', () => {
            this.isDragging = false;
        });

        container.addEventListener('wheel', (e) => {
            e.preventDefault();
            const zoomFactor = 1.1;
            const mouseX = e.offsetX;
            const mouseY = e.offsetY;

            let newScale = (e.deltaY < 0) ? this.scale * zoomFactor : this.scale / zoomFactor;
            newScale = Math.min(Math.max(newScale, 0.3), 3.0);

            // Zoom centered on cursor
            this.panX -= (mouseX - this.panX) * (newScale / this.scale - 1);
            this.panY -= (mouseY - this.panY) * (newScale / this.scale - 1);
            this.scale = newScale;

            this.updateTransform();
        }, { passive: false });
    }

    updateTransform() {
        this.viewportGroup.setAttribute('transform', 
            `translate(${this.panX}, ${this.panY}) scale(${this.scale})`
        );
    }

    zoomIn() {
        this.scale = Math.min(this.scale * 1.2, 3.0);
        this.updateTransform();
    }

    zoomOut() {
        this.scale = Math.max(this.scale / 1.2, 0.3);
        this.updateTransform();
    }

    resetView() {
        this.scale = 1.0;
        this.panX = 0;
        this.panY = 0;
        this.centerTree();
        this.updateTransform();
    }

    centerTree() {
        if (!this.currentTree || !this.currentTree.root) {
            this.panX = 0;
            this.panY = 0;
            this.updateTransform();
            return;
        }

        const rect = this.svg.getBoundingClientRect();
        let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;

        this.nodeCoordinates.forEach(pos => {
            if (pos.x < minX) minX = pos.x;
            if (pos.x > maxX) maxX = pos.x;
            if (pos.y < minY) minY = pos.y;
            if (pos.y > maxY) maxY = pos.y;
        });

        if (minX !== Infinity) {
            const treeWidth = maxX - minX;
            const treeCenterX = (minX + maxX) / 2;
            this.panX = (rect.width / 2) - treeCenterX;
            this.panY = 60; // top padding
        }
        this.updateTransform();
    }

    /**
     * Inorder-based aesthetic tree layout calculation.
     * Guarantees that x(left) < x(parent) < x(right) with zero overlapping.
     */
    computeLayout(root) {
        this.nodeCoordinates.clear();
        if (!root) return;

        let index = 0;
        const levelGap = 85;
        const nodeSpacing = 68;

        function traverseInorder(node, depth, coords) {
            if (!node) return;
            traverseInorder(node.left, depth + 1, coords);

            const x = index * nodeSpacing + 60;
            const y = depth * levelGap + 50;
            index++;

            coords.set(node.value, {
                x: x,
                y: y,
                depth: depth,
                data: node,
                id: node.id
            });

            traverseInorder(node.right, depth + 1, coords);
        }

        traverseInorder(root, 0, this.nodeCoordinates);
    }

    /**
     * Render the complete tree snapshot into the SVG layers.
     */
    renderTree(treeData) {
        this.currentTree = treeData;
        this.edgesLayer.innerHTML = '';
        this.nodesLayer.innerHTML = '';
        this.particlesLayer.innerHTML = '';
        this.nodeElements.clear();
        this.edgeElements.clear();

        const emptyState = document.getElementById('empty-state');
        if (!treeData || !treeData.root) {
            if (emptyState) emptyState.classList.remove('hidden');
            return;
        }
        if (emptyState) emptyState.classList.add('hidden');

        // 1. Compute coordinates
        this.computeLayout(treeData.root);

        // 2. Render connecting constellation edges
        this.renderEdges(treeData.root);

        // 3. Render celestial star nodes
        this.renderNodes(treeData.root);

        // Auto-center on first render
        this.centerTree();
    }

    renderEdges(node) {
        if (!node) return;

        const parentPos = this.nodeCoordinates.get(node.value);

        if (node.left) {
            const leftPos = this.nodeCoordinates.get(node.left.value);
            this.createEdge(parentPos, leftPos, node.value, node.left.value);
            this.renderEdges(node.left);
        }

        if (node.right) {
            const rightPos = this.nodeCoordinates.get(node.right.value);
            this.createEdge(parentPos, rightPos, node.value, node.right.value);
            this.renderEdges(node.right);
        }
    }

    createEdge(fromPos, toPos, fromVal, toVal) {
        const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        line.setAttribute('x1', fromPos.x);
        line.setAttribute('y1', fromPos.y);
        line.setAttribute('x2', toPos.x);
        line.setAttribute('y2', toPos.y);
        line.setAttribute('class', 'constellation-edge');
        line.setAttribute('id', `edge-${fromVal}-${toVal}`);

        this.edgesLayer.appendChild(line);
        this.edgeElements.set(`${fromVal}-${toVal}`, line);
    }

    renderNodes(node) {
        if (!node) return;

        const pos = this.nodeCoordinates.get(node.value);

        const group = document.createElementNS('http://www.w3.org/2000/svg', 'g');
        group.setAttribute('class', 'star-group');
        group.setAttribute('transform', `translate(${pos.x}, ${pos.y})`);
        group.setAttribute('data-value', node.value);
        group.setAttribute('data-id', node.id);

        // Outer pulsing orbital ring
        const orbitalRing = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        orbitalRing.setAttribute('class', 'outer-orbital-ring');
        orbitalRing.setAttribute('r', '27');

        // Stellar body (core plasma)
        const corona = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        corona.setAttribute('class', 'star-corona');
        corona.setAttribute('r', '19');

        // Radial gradient styling based on balance factor
        const bf = node.balanceFactor || 0;
        let grad = 'url(#plasma-cyan)';
        let pillColor = 'var(--neon-emerald)';
        if (Math.abs(bf) === 1) {
            pillColor = 'var(--neon-gold)';
        } else if (Math.abs(bf) > 1) {
            grad = 'url(#plasma-crimson)';
            pillColor = 'var(--neon-red)';
        }
        corona.setAttribute('fill', grad);

        // Star key text
        const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        text.setAttribute('class', 'star-key-text');
        text.textContent = node.value;

        // Satellite Pill Badge for Balance Factor (BF: 0 / H: 2)
        const pillGroup = document.createElementNS('http://www.w3.org/2000/svg', 'g');
        pillGroup.setAttribute('class', 'satellite-pill');
        pillGroup.setAttribute('transform', 'translate(14, -22)');

        const pillRect = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        pillRect.setAttribute('class', 'satellite-rect');
        pillRect.setAttribute('x', '-12');
        pillRect.setAttribute('y', '-8');
        pillRect.setAttribute('width', '38');
        pillRect.setAttribute('height', '16');
        pillRect.setAttribute('style', `stroke: ${pillColor};`);

        const pillText = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        pillText.setAttribute('class', 'satellite-text');
        pillText.setAttribute('x', '7');
        pillText.setAttribute('y', '0');
        pillText.setAttribute('style', `fill: ${pillColor};`);
        pillText.textContent = `BF:${bf >= 0 ? '+' : ''}${bf}`;

        pillGroup.appendChild(pillRect);
        pillGroup.appendChild(pillText);

        // Assemble group
        group.appendChild(orbitalRing);
        group.appendChild(corona);
        group.appendChild(text);
        group.appendChild(pillGroup);

        // Click handler for entity inspection
        group.addEventListener('click', (e) => {
            e.stopPropagation();
            this.highlightNodeSelection(node.value);
            if (this.onNodeClick) {
                this.onNodeClick(node);
            }
        });

        this.nodesLayer.appendChild(group);
        this.nodeElements.set(node.value, group);

        // Recurse children
        this.renderNodes(node.left);
        this.renderNodes(node.right);
    }

    highlightNodeSelection(value) {
        document.querySelectorAll('.star-group').forEach(g => g.classList.remove('active-target'));
        const el = this.nodeElements.get(value);
        if (el) {
            el.classList.add('active-target');
        }
    }

    clearHighlights() {
        document.querySelectorAll('.star-group').forEach(g => {
            g.classList.remove('active-sweep', 'active-target', 'rotation-pivot');
        });
        document.querySelectorAll('.constellation-edge').forEach(e => {
            e.classList.remove('highlighted');
        });
    }

    /**
     * Animate a search sweep across visited star nodes
     */
    async animateSearchPath(path, found, speedMultiplier = 1.0) {
        this.clearHighlights();
        const baseDelay = 450 / speedMultiplier;

        for (let i = 0; i < path.length; i++) {
            const val = path[i];
            const nodeEl = this.nodeElements.get(val);

            if (nodeEl) {
                nodeEl.classList.add('active-sweep');
                // Play pulse sound via event or hook
                if (window.AudioEngine) window.AudioEngine.playNodeHover(val);
            }

            // Highlight edge to previous node in path
            if (i > 0) {
                const prevVal = path[i - 1];
                const edgeKey1 = `${prevVal}-${val}`;
                const edgeKey2 = `${val}-${prevVal}`;
                const edgeEl = this.edgeElements.get(edgeKey1) || this.edgeElements.get(edgeKey2);
                if (edgeEl) edgeEl.classList.add('highlighted');
            }

            await new Promise(r => setTimeout(r, baseDelay));
            if (nodeEl && i < path.length - 1) {
                nodeEl.classList.remove('active-sweep');
            }
        }

        // Final target highlight
        const lastVal = path[path.length - 1];
        const lastEl = this.nodeElements.get(lastVal);
        if (lastEl) {
            lastEl.classList.remove('active-sweep');
            if (found) {
                lastEl.classList.add('active-target');
                if (window.AudioEngine) window.AudioEngine.playSuccess();
            } else {
                if (window.AudioEngine) window.AudioEngine.playError();
            }
        }
    }

    /**
     * Animate a 2-children deletion swap:
     * Highlights target node and inorder successor, draws an energetic beam between them,
     * glides a transfer particle carrying the successor's key to replace the target,
     * and dissolves the old successor position!
     */
    async animateSwap(targetVal, successorVal, speedMultiplier = 1.0) {
        const targetEl = this.nodeElements.get(targetVal);
        const successorEl = this.nodeElements.get(successorVal);
        const targetPos = this.nodeCoordinates.get(targetVal);
        const successorPos = this.nodeCoordinates.get(successorVal);

        if (!targetEl || !successorEl || !targetPos || !successorPos) return;

        // Step 1: Highlight target in crimson and successor in gold
        targetEl.classList.add('collapsing-target');
        successorEl.classList.add('inorder-successor');

        if (window.AudioEngine) window.AudioEngine.playNodeHover(successorVal);

        // Step 2: Draw connecting curved energy beam
        const path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const midX = (targetPos.x + successorPos.x) / 2 - 35;
        const midY = (targetPos.y + successorPos.y) / 2;
        const d = `M ${successorPos.x} ${successorPos.y} Q ${midX} ${midY} ${targetPos.x} ${targetPos.y}`;
        path.setAttribute('d', d);
        path.setAttribute('class', 'swap-energy-beam');
        this.particlesLayer.appendChild(path);

        // Step 3: Create floating transfer entity carrying the successor value
        const orbGroup = document.createElementNS('http://www.w3.org/2000/svg', 'g');
        const orb = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        orb.setAttribute('r', '15');
        orb.setAttribute('class', 'transfer-particle');

        const orbText = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        orbText.setAttribute('class', 'star-key-text');
        orbText.setAttribute('style', 'font-size: 11px; fill: #000000; font-weight: 800;');
        orbText.textContent = successorVal;

        orbGroup.appendChild(orb);
        orbGroup.appendChild(orbText);
        orbGroup.setAttribute('transform', `translate(${successorPos.x}, ${successorPos.y})`);
        this.particlesLayer.appendChild(orbGroup);

        const duration = 1200 / speedMultiplier;
        const startTime = performance.now();

        // Animate along curve
        await new Promise(resolve => {
            function step(now) {
                const elapsed = now - startTime;
                const t = Math.min(elapsed / duration, 1.0);

                // Quadratic bezier interpolation
                const curX = Math.pow(1 - t, 2) * successorPos.x + 2 * (1 - t) * t * midX + Math.pow(t, 2) * targetPos.x;
                const curY = Math.pow(1 - t, 2) * successorPos.y + 2 * (1 - t) * t * midY + Math.pow(t, 2) * targetPos.y;
                orbGroup.setAttribute('transform', `translate(${curX}, ${curY})`);

                if (t < 1.0) {
                    requestAnimationFrame(step);
                } else {
                    resolve();
                }
            }
            requestAnimationFrame(step);
        });

        // Step 4: Flash target text to successor value
        const targetText = targetEl.querySelector('.star-key-text');
        if (targetText) {
            targetText.textContent = successorVal;
        }
        if (window.AudioEngine) window.AudioEngine.playSuccess();

        // Clean up animation elements
        path.remove();
        orbGroup.remove();

        await new Promise(r => setTimeout(r, 450 / speedMultiplier));

        targetEl.classList.remove('collapsing-target');
        successorEl.classList.remove('inorder-successor');
    }

    /**
     * Enhanced animation for AVL Rotations:
     * Draws rotational flux vector, marks unbalanced root and pivot nodes,
     * and shows directional orbital movement!
     */
    async animateRotation(rotationEvent, speedMultiplier = 1.0) {
        const rootEl = this.nodeElements.get(rotationEvent.rootValue);
        const pivotEl = this.nodeElements.get(rotationEvent.pivotValue);
        const rootPos = this.nodeCoordinates.get(rotationEvent.rootValue);
        const pivotPos = this.nodeCoordinates.get(rotationEvent.pivotValue);

        if (rootEl) rootEl.classList.add('rotation-pivot');
        if (pivotEl) pivotEl.classList.add('active-sweep');

        let fluxArc = null;
        if (rootPos && pivotPos) {
            fluxArc = document.createElementNS('http://www.w3.org/2000/svg', 'path');
            const sweepFlag = (rotationEvent.type === 'LL' || rotationEvent.direction === 'clockwise') ? 0 : 1;
            const radius = Math.hypot(rootPos.x - pivotPos.x, rootPos.y - pivotPos.y);
            const d = `M ${rootPos.x} ${rootPos.y} A ${radius} ${radius} 0 0 ${sweepFlag} ${pivotPos.x} ${pivotPos.y}`;
            fluxArc.setAttribute('d', d);
            fluxArc.setAttribute('class', 'rotation-flux-arc');
            this.particlesLayer.appendChild(fluxArc);
        }

        if (window.AudioEngine) window.AudioEngine.playWarp();

        // Keep highlight and rotation arc visible for comprehension
        const displayTime = 1400 / speedMultiplier;
        await new Promise(r => setTimeout(r, displayTime));

        if (fluxArc) fluxArc.remove();
        if (rootEl) rootEl.classList.remove('rotation-pivot');
        if (pivotEl) pivotEl.classList.remove('active-sweep');
    }

    /**
     * Animate tree traversal sequence (inorder, preorder, postorder)
     */
    async animateTraversal(sequence, speedMultiplier = 1.0, onStep = null) {
        this.clearHighlights();
        const baseDelay = 400 / speedMultiplier;

        for (let i = 0; i < sequence.length; i++) {
            const val = sequence[i];
            const nodeEl = this.nodeElements.get(val);

            if (nodeEl) {
                nodeEl.classList.add('active-sweep');
                if (window.AudioEngine) window.AudioEngine.playNodeHover(val);
            }

            if (onStep) {
                onStep(val, i);
            }

            await new Promise(r => setTimeout(r, baseDelay));

            if (nodeEl) {
                nodeEl.classList.remove('active-sweep');
                nodeEl.classList.add('active-target'); // retain faint visited glow
            }
        }

        if (window.AudioEngine) window.AudioEngine.playSuccess();
    }
}

