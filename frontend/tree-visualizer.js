





class TreeVisualizer {
    constructor(svgElementId, options = {}) {
        this.svg = document.getElementById(svgElementId);
        this.viewportGroup = document.getElementById('viewport-group');
        this.edgesLayer = document.getElementById('edges-layer');
        this.nodesLayer = document.getElementById('nodes-layer');
        this.particlesLayer = document.getElementById('particles-layer');
        this.onNodeClick = options.onNodeClick || null;


        this.scale = 1.0;
        this.panX = 0;
        this.panY = 0;
        this.isDragging = false;
        this.startX = 0;
        this.startY = 0;


        this.currentTree = null;
        this.nodeCoordinates = new Map();
        this.nodeElements = new Map();
        this.edgeElements = new Map();
        this.initPanZoom();
    }
    initPanZoom() {
        const container = this.svg.parentElement;
        container.addEventListener('mousedown', (e) => {
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
            this.panY = 60;
        }
        this.updateTransform();
    }





    /**
     * Reingold-Tilford / Buchheim aesthetic binary tree layout.
     * Guarantees:
     * 1. Left child is strictly to the left: x(left) < x(parent)
     * 2. Right child is strictly to the right: x(right) > x(parent)
     * 3. Parent with two children is centered EXACTLY between them:
     *    x(parent) = (x(left) + x(right)) / 2
     * 4. Subtrees at any depth maintain minimum horizontal separation (no overlapping).
     * 5. Clean, symmetric, classic binary tree shape.
     */
    computeLayout(root) {
        this.nodeCoordinates.clear();
        if (!root) return;

        const levelGap = 85;       // vertical distance between levels
        const minDistance = 72;    // minimum horizontal distance between node centers

        // Step 1: Bottom-up layout of subtrees relative to each subtree's root (relX = 0)
        function buildLayout(node) {
            if (!node) return null;

            const left = buildLayout(node.left);
            const right = buildLayout(node.right);

            // Leaf node: placed at relX = 0
            if (!left && !right) {
                return {
                    node: node,
                    relX: 0,
                    leftContour: [0],
                    rightContour: [0],
                    left: null,
                    right: null
                };
            }

            // Only left child
            if (left && !right) {
                const offset = -minDistance * 0.75;
                shiftTree(left, offset);
                return {
                    node: node,
                    relX: 0,
                    leftContour: [0, ...left.leftContour],
                    rightContour: [0, ...left.rightContour],
                    left: left,
                    right: null
                };
            }

            // Only right child
            if (!left && right) {
                const offset = minDistance * 0.75;
                shiftTree(right, offset);
                return {
                    node: node,
                    relX: 0,
                    leftContour: [0, ...right.leftContour],
                    rightContour: [0, ...right.rightContour],
                    left: null,
                    right: right
                };
            }

            // Both children exist:
            // Calculate minimum distance needed so left and right subtrees do not collide at any depth
            let maxOverlap = 0;
            const checkDepth = Math.min(left.rightContour.length, right.leftContour.length);
            for (let d = 0; d < checkDepth; d++) {
                const diff = left.rightContour[d] - right.leftContour[d];
                if (diff + minDistance > maxOverlap) {
                    maxOverlap = diff + minDistance;
                }
            }

            const separation = Math.max(minDistance, maxOverlap);

            // Center parent between left and right subtrees
            shiftTree(left, -separation / 2);
            shiftTree(right, separation / 2);

            // Merge contours
            const leftContour = [0];
            const rightContour = [0];
            const maxD = Math.max(left.leftContour.length, right.leftContour.length);
            for (let d = 0; d < maxD; d++) {
                const lc = (d < left.leftContour.length)
                    ? left.leftContour[d]
                    : right.leftContour[d];
                const rc = (d < right.rightContour.length)
                    ? right.rightContour[d]
                    : left.rightContour[d];
                leftContour.push(lc);
                rightContour.push(rc);
            }

            return {
                node: node,
                relX: 0,
                leftContour: leftContour,
                rightContour: rightContour,
                left: left,
                right: right
            };
        }

        function shiftTree(item, dx) {
            if (!item) return;
            item.relX += dx;
            for (let i = 0; i < item.leftContour.length; i++) item.leftContour[i] += dx;
            for (let i = 0; i < item.rightContour.length; i++) item.rightContour[i] += dx;
            shiftTree(item.left, dx);
            shiftTree(item.right, dx);
        }

        const layoutRoot = buildLayout(root);
        if (!layoutRoot) return;

        // Step 2: Find minimum X to shift everything positive
        let minX = Infinity;
        function findMinX(item, currentX) {
            if (!item) return;
            const x = currentX + item.relX;
            if (x < minX) minX = x;
            findMinX(item.left, x);
            findMinX(item.right, x);
        }
        findMinX(layoutRoot, 0);

        const xShift = (minX < 80) ? (80 - minX) : 80;

        const coords = this.nodeCoordinates;
        function assignCoords(item, parentX, depth) {
            if (!item) return;
            const x = parentX + item.relX;
            const y = depth * levelGap + 60;
            coords.set(item.node.value, {
                x: x,
                y: y,
                depth: depth,
                data: item.node,
                id: item.node.id
            });
            assignCoords(item.left, x, depth + 1);
            assignCoords(item.right, x, depth + 1);
        }

        assignCoords(layoutRoot, xShift, 0);
    }

    renderTree(treeData) {
        return new Promise(resolve => {
            if (this._animFrameId) {
                cancelAnimationFrame(this._animFrameId);
                this._animFrameId = null;
            }

            this.currentTree = treeData;
            const emptyState = document.getElementById('empty-state');
            if (!treeData || !treeData.root) {
                this.edgesLayer.innerHTML = '';
                this.nodesLayer.innerHTML = '';
                this.particlesLayer.innerHTML = '';
                this.nodeElements.clear();
                this.edgeElements.clear();
                this.nodeCoordinates.clear();
                if (emptyState) emptyState.classList.remove('hidden');
                resolve();
                return;
            }
            if (emptyState) emptyState.classList.add('hidden');

            const oldPositions = new Map();
            this.nodeCoordinates.forEach((pos, val) => {
                oldPositions.set(val, { x: pos.x, y: pos.y });
            });

            this.computeLayout(treeData.root);
            const newPositions = new Map();
            this.nodeCoordinates.forEach((pos, val) => {
                newPositions.set(val, { x: pos.x, y: pos.y });
            });

            if (oldPositions.size === 0 || this.nodeElements.size === 0) {
                this.edgesLayer.innerHTML = '';
                this.nodesLayer.innerHTML = '';
                this.particlesLayer.innerHTML = '';
                this.nodeElements.clear();
                this.edgeElements.clear();
                this.renderEdges(treeData.root);
                this.renderNodes(treeData.root);
                this.centerTree();
                resolve();
                return;
            }

            this._tweenNodes(oldPositions, newPositions, treeData, resolve);
        });
    }

    _tweenNodes(oldPositions, newPositions, newTree, resolve) {
        const newNodeMap = new Map();
        const parentMap = new Map();
        function collect(node, pVal) {
            if (!node) return;
            newNodeMap.set(node.value, node);
            if (pVal !== null) parentMap.set(node.value, pVal);
            collect(node.left, node.value);
            collect(node.right, node.value);
        }
        collect(newTree.root, null);

        const duration = 400;
        const easeOutCubic = t => 1 - Math.pow(1 - t, 3);

        const existing = [];
        const added = [];
        const removed = [];

        newPositions.forEach((pos, val) => {
            if (oldPositions.has(val)) {
                existing.push(val);
            } else {
                added.push(val);
            }
        });
        oldPositions.forEach((pos, val) => {
            if (!newPositions.has(val)) {
                removed.push(val);
            }
        });

        const renderStartPos = new Map();
        existing.forEach(val => renderStartPos.set(val, oldPositions.get(val)));
        added.forEach(val => {
            const pVal = parentMap.get(val);
            let sPos = (pVal !== undefined && oldPositions.has(pVal))
                ? oldPositions.get(pVal)
                : (pVal !== undefined && newPositions.has(pVal))
                    ? newPositions.get(pVal)
                    : newPositions.get(val);
            renderStartPos.set(val, { x: sPos.x, y: sPos.y });
        });

        this.edgesLayer.innerHTML = '';
        this.nodesLayer.innerHTML = '';
        this.particlesLayer.innerHTML = '';
        this.nodeElements.clear();
        this.edgeElements.clear();

        const currentPos = new Map();
        renderStartPos.forEach((p, v) => currentPos.set(v, { x: p.x, y: p.y }));

        renderStartPos.forEach((p, v) => {
            this.nodeCoordinates.set(v, { x: p.x, y: p.y, data: newNodeMap.get(v) });
        });
        this.renderEdges(newTree.root);
        this.renderNodes(newTree.root);

        added.forEach(val => {
            const el = this.nodeElements.get(val);
            if (el) el.setAttribute('opacity', '0');
        });

        const removedEls = [];
        removed.forEach(val => {
            const sPos = oldPositions.get(val);
            const group = document.createElementNS('http://www.w3.org/2000/svg', 'g');
            group.setAttribute('class', 'star-group');
            group.setAttribute('transform', `translate(${sPos.x}, ${sPos.y})`);
            const corona = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
            corona.setAttribute('class', 'star-corona');
            corona.setAttribute('r', '19');
            corona.setAttribute('fill', 'url(#plasma-crimson)');
            const txt = document.createElementNS('http://www.w3.org/2000/svg', 'text');
            txt.setAttribute('class', 'star-key-text');
            txt.textContent = val;
            group.appendChild(corona);
            group.appendChild(txt);
            this.nodesLayer.appendChild(group);
            removedEls.push(group);
        });

        const startTime = performance.now();

        const animate = (now) => {
            const elapsed = now - startTime;
            const progress = Math.min(elapsed / duration, 1.0);
            const t = easeOutCubic(progress);

            existing.forEach(val => {
                const el = this.nodeElements.get(val);
                const oP = oldPositions.get(val);
                const nP = newPositions.get(val);
                if (!el || !oP || !nP) return;
                const cx = oP.x + (nP.x - oP.x) * t;
                const cy = oP.y + (nP.y - oP.y) * t;
                el.setAttribute('transform', `translate(${cx}, ${cy})`);
                currentPos.set(val, { x: cx, y: cy });
            });

            added.forEach(val => {
                const el = this.nodeElements.get(val);
                const sP = renderStartPos.get(val);
                const nP = newPositions.get(val);
                if (!el || !sP || !nP) return;
                const cx = sP.x + (nP.x - sP.x) * t;
                const cy = sP.y + (nP.y - sP.y) * t;
                el.setAttribute('transform', `translate(${cx}, ${cy})`);
                el.setAttribute('opacity', String(t));
                currentPos.set(val, { x: cx, y: cy });
            });

            removedEls.forEach(el => {
                el.setAttribute('opacity', String(Math.max(0, 1 - t * 1.5)));
            });

            this.edgeElements.forEach((line, key) => {
                const parts = key.split('-');
                const pVal = parseInt(parts[0], 10);
                const cVal = parseInt(parts[1], 10);
                const pPos = currentPos.get(pVal);
                const cPos = currentPos.get(cVal);
                if (pPos) {
                    line.setAttribute('x1', pPos.x);
                    line.setAttribute('y1', pPos.y);
                }
                if (cPos) {
                    line.setAttribute('x2', cPos.x);
                    line.setAttribute('y2', cPos.y);
                }
            });

            if (progress < 1.0) {
                this._animFrameId = requestAnimationFrame(animate);
            } else {
                this._animFrameId = null;

                newPositions.forEach((nP, val) => {
                    const el = this.nodeElements.get(val);
                    if (el) {
                        el.setAttribute('transform', `translate(${nP.x}, ${nP.y})`);
                        el.setAttribute('opacity', '1');
                    }
                });

                this.edgeElements.forEach((line, key) => {
                    const parts = key.split('-');
                    const pVal = parseInt(parts[0], 10);
                    const cVal = parseInt(parts[1], 10);
                    const pPos = newPositions.get(pVal);
                    const cPos = newPositions.get(cVal);
                    if (pPos) {
                        line.setAttribute('x1', pPos.x);
                        line.setAttribute('y1', pPos.y);
                    }
                    if (cPos) {
                        line.setAttribute('x2', cPos.x);
                        line.setAttribute('y2', cPos.y);
                    }
                });

                removedEls.forEach(el => el.remove());

                this.nodeCoordinates.clear();
                newPositions.forEach((p, val) => {
                    this.nodeCoordinates.set(val, { x: p.x, y: p.y, data: newNodeMap.get(val) });
                });

                this.centerTree();
                if (resolve) resolve();
            }
        };

        this._animFrameId = requestAnimationFrame(animate);
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


        const orbitalRing = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        orbitalRing.setAttribute('class', 'outer-orbital-ring');
        orbitalRing.setAttribute('r', '27');


        const corona = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
        corona.setAttribute('class', 'star-corona');
        corona.setAttribute('r', '19');


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


        const text = document.createElementNS('http://www.w3.org/2000/svg', 'text');
        text.setAttribute('class', 'star-key-text');
        text.textContent = node.value;


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


        group.appendChild(orbitalRing);
        group.appendChild(corona);
        group.appendChild(text);
        group.appendChild(pillGroup);


        group.addEventListener('click', (e) => {
            e.stopPropagation();
            this.highlightNodeSelection(node.value);
            if (this.onNodeClick) {
                this.onNodeClick(node);
            }
        });
        this.nodesLayer.appendChild(group);
        this.nodeElements.set(node.value, group);


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




    async animateSearchPath(path, found, speedMultiplier = 1.0) {
        this.clearHighlights();
        const baseDelay = 450 / speedMultiplier;
        for (let i = 0; i < path.length; i++) {
            const val = path[i];
            const nodeEl = this.nodeElements.get(val);
            if (nodeEl) {
                nodeEl.classList.add('active-sweep');
                if (window.AudioEngine) window.AudioEngine.playNodeHover(val);
            }


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







    async animateSwap(targetVal, successorVal, speedMultiplier = 1.0) {
        const targetEl = this.nodeElements.get(targetVal);
        const successorEl = this.nodeElements.get(successorVal);
        const targetPos = this.nodeCoordinates.get(targetVal);
        const successorPos = this.nodeCoordinates.get(successorVal);
        if (!targetEl || !successorEl || !targetPos || !successorPos) return;


        targetEl.classList.add('collapsing-target');
        successorEl.classList.add('inorder-successor');
        if (window.AudioEngine) window.AudioEngine.playNodeHover(successorVal);


        const path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
        const midX = (targetPos.x + successorPos.x) / 2 - 35;
        const midY = (targetPos.y + successorPos.y) / 2;
        const d = `M ${successorPos.x} ${successorPos.y} Q ${midX} ${midY} ${targetPos.x} ${targetPos.y}`;
        path.setAttribute('d', d);
        path.setAttribute('class', 'swap-energy-beam');
        this.particlesLayer.appendChild(path);


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


        await new Promise(resolve => {
            function step(now) {
                const elapsed = now - startTime;
                const t = Math.min(elapsed / duration, 1.0);


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


        const targetText = targetEl.querySelector('.star-key-text');
        if (targetText) {
            const bounceDuration = 300 / speedMultiplier;
            const bounceStart = performance.now();
            await new Promise(resolve => {
                function bounceStep(now) {
                    const bt = Math.min((now - bounceStart) / bounceDuration, 1.0);
                    let scale;
                    if (bt < 0.5) {
                        scale = 0.5 + (1.5 - 0.5) * (bt / 0.5);
                    } else {
                        scale = 1.5 + (1.0 - 1.5) * ((bt - 0.5) / 0.5);
                    }
                    targetText.setAttribute('transform', `scale(${scale})`);
                    if (bt < 1.0) {
                        requestAnimationFrame(bounceStep);
                    } else {
                        targetText.setAttribute('transform', '');
                        resolve();
                    }
                }
                targetText.textContent = successorVal;
                requestAnimationFrame(bounceStep);
            });
        }
        if (window.AudioEngine) window.AudioEngine.playSuccess();


        const targetCorona = targetEl.querySelector('.star-corona');
        if (targetCorona) {
            const origStroke = targetCorona.style.stroke;
            const origFilter = targetCorona.style.filter;
            targetCorona.style.stroke = '#39ff14';
            targetCorona.style.filter = 'drop-shadow(0 0 24px #39ff14)';
            await new Promise(r => setTimeout(r, 300 / speedMultiplier));
            targetCorona.style.stroke = origStroke;
            targetCorona.style.filter = origFilter;
        }


        path.remove();
        orbGroup.remove();
        await new Promise(r => setTimeout(r, 450 / speedMultiplier));
        targetEl.classList.remove('collapsing-target');
        successorEl.classList.remove('inorder-successor');
    }






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

        // Hold visual rotation indicator briefly for comprehension
        await new Promise(r => setTimeout(r, 600 / speedMultiplier));

        if (fluxArc) fluxArc.remove();
        if (rootEl) rootEl.classList.remove('rotation-pivot');
        if (pivotEl) pivotEl.classList.remove('active-sweep');
    }




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
                nodeEl.classList.add('active-target');
            }
        }
        if (window.AudioEngine) window.AudioEngine.playSuccess();
    }









    async animateSplayStep(step, speedMultiplier = 1.0) {
        const accessedEl = this.nodeElements.get(step.accessedValue);
        const newRootEl = this.nodeElements.get(step.newRootValue);
        const accessedPos = this.nodeCoordinates.get(step.accessedValue);
        const newRootPos = this.nodeCoordinates.get(step.newRootValue);


        if (accessedEl) {
            accessedEl.classList.add('splay-step');
            if (window.AudioEngine) window.AudioEngine.playNodeHover(step.accessedValue);
        }


        let arc = null;
        if (accessedPos && newRootPos && accessedEl && newRootEl) {
            arc = document.createElementNS('http://www.w3.org/2000/svg', 'path');
            const midX = (accessedPos.x + newRootPos.x) / 2;
            const midY = Math.min(accessedPos.y, newRootPos.y) - 30;
            arc.setAttribute('d', `M ${accessedPos.x} ${accessedPos.y} Q ${midX} ${midY} ${newRootPos.x} ${newRootPos.y}`);
            arc.setAttribute('fill', 'none');
            arc.setAttribute('stroke', '#bd00ff');
            arc.setAttribute('stroke-width', '2');
            arc.setAttribute('stroke-dasharray', '5 3');
            arc.setAttribute('opacity', '0.75');
            arc.setAttribute('style', 'filter: drop-shadow(0 0 8px #bd00ff);');
            this.particlesLayer.appendChild(arc);
        }
        await new Promise(r => setTimeout(r, 400 / speedMultiplier));


        if (accessedEl) accessedEl.classList.remove('splay-step');
        if (newRootEl) {
            newRootEl.classList.add('splay-new-root');
            if (window.AudioEngine) window.AudioEngine.playNodeHover(step.newRootValue);
        }
        await new Promise(r => setTimeout(r, 300 / speedMultiplier));


        if (newRootEl) newRootEl.classList.remove('splay-new-root');
        if (arc) arc.remove();
    }
}

