
(function(l, r) { if (!l || l.getElementById('livereloadscript')) return; r = l.createElement('script'); r.async = 1; r.src = '//' + (self.location.host || 'localhost').split(':')[0] + ':35729/livereload.js?snipver=1'; r.id = 'livereloadscript'; l.getElementsByTagName('head')[0].appendChild(r) })(self.document);
var app = (function () {
    'use strict';

    function noop() { }
    function assign(tar, src) {
        // @ts-ignore
        for (const k in src)
            tar[k] = src[k];
        return tar;
    }
    function add_location(element, file, line, column, char) {
        element.__svelte_meta = {
            loc: { file, line, column, char }
        };
    }
    function run(fn) {
        return fn();
    }
    function blank_object() {
        return Object.create(null);
    }
    function run_all(fns) {
        fns.forEach(run);
    }
    function is_function(thing) {
        return typeof thing === 'function';
    }
    function safe_not_equal(a, b) {
        return a != a ? b == b : a !== b || ((a && typeof a === 'object') || typeof a === 'function');
    }
    function is_empty(obj) {
        return Object.keys(obj).length === 0;
    }
    function validate_store(store, name) {
        if (store != null && typeof store.subscribe !== 'function') {
            throw new Error(`'${name}' is not a store with a 'subscribe' method`);
        }
    }
    function subscribe(store, ...callbacks) {
        if (store == null) {
            return noop;
        }
        const unsub = store.subscribe(...callbacks);
        return unsub.unsubscribe ? () => unsub.unsubscribe() : unsub;
    }
    function component_subscribe(component, store, callback) {
        component.$$.on_destroy.push(subscribe(store, callback));
    }
    function create_slot(definition, ctx, $$scope, fn) {
        if (definition) {
            const slot_ctx = get_slot_context(definition, ctx, $$scope, fn);
            return definition[0](slot_ctx);
        }
    }
    function get_slot_context(definition, ctx, $$scope, fn) {
        return definition[1] && fn
            ? assign($$scope.ctx.slice(), definition[1](fn(ctx)))
            : $$scope.ctx;
    }
    function get_slot_changes(definition, $$scope, dirty, fn) {
        if (definition[2] && fn) {
            const lets = definition[2](fn(dirty));
            if ($$scope.dirty === undefined) {
                return lets;
            }
            if (typeof lets === 'object') {
                const merged = [];
                const len = Math.max($$scope.dirty.length, lets.length);
                for (let i = 0; i < len; i += 1) {
                    merged[i] = $$scope.dirty[i] | lets[i];
                }
                return merged;
            }
            return $$scope.dirty | lets;
        }
        return $$scope.dirty;
    }
    function update_slot_base(slot, slot_definition, ctx, $$scope, slot_changes, get_slot_context_fn) {
        if (slot_changes) {
            const slot_context = get_slot_context(slot_definition, ctx, $$scope, get_slot_context_fn);
            slot.p(slot_context, slot_changes);
        }
    }
    function get_all_dirty_from_scope($$scope) {
        if ($$scope.ctx.length > 32) {
            const dirty = [];
            const length = $$scope.ctx.length / 32;
            for (let i = 0; i < length; i++) {
                dirty[i] = -1;
            }
            return dirty;
        }
        return -1;
    }
    function append(target, node) {
        target.appendChild(node);
    }
    function insert(target, node, anchor) {
        target.insertBefore(node, anchor || null);
    }
    function detach(node) {
        node.parentNode.removeChild(node);
    }
    function destroy_each(iterations, detaching) {
        for (let i = 0; i < iterations.length; i += 1) {
            if (iterations[i])
                iterations[i].d(detaching);
        }
    }
    function element(name) {
        return document.createElement(name);
    }
    function text(data) {
        return document.createTextNode(data);
    }
    function space() {
        return text(' ');
    }
    function empty() {
        return text('');
    }
    function listen(node, event, handler, options) {
        node.addEventListener(event, handler, options);
        return () => node.removeEventListener(event, handler, options);
    }
    function attr(node, attribute, value) {
        if (value == null)
            node.removeAttribute(attribute);
        else if (node.getAttribute(attribute) !== value)
            node.setAttribute(attribute, value);
    }
    function children(element) {
        return Array.from(element.childNodes);
    }
    function set_input_value(input, value) {
        input.value = value == null ? '' : value;
    }
    function custom_event(type, detail, bubbles = false) {
        const e = document.createEvent('CustomEvent');
        e.initCustomEvent(type, bubbles, false, detail);
        return e;
    }

    let current_component;
    function set_current_component(component) {
        current_component = component;
    }
    function get_current_component() {
        if (!current_component)
            throw new Error('Function called outside component initialization');
        return current_component;
    }
    function onMount(fn) {
        get_current_component().$$.on_mount.push(fn);
    }
    function afterUpdate(fn) {
        get_current_component().$$.after_update.push(fn);
    }
    function onDestroy(fn) {
        get_current_component().$$.on_destroy.push(fn);
    }
    function createEventDispatcher() {
        const component = get_current_component();
        return (type, detail) => {
            const callbacks = component.$$.callbacks[type];
            if (callbacks) {
                // TODO are there situations where events could be dispatched
                // in a server (non-DOM) environment?
                const event = custom_event(type, detail);
                callbacks.slice().forEach(fn => {
                    fn.call(component, event);
                });
            }
        };
    }

    const dirty_components = [];
    const binding_callbacks = [];
    const render_callbacks = [];
    const flush_callbacks = [];
    const resolved_promise = Promise.resolve();
    let update_scheduled = false;
    function schedule_update() {
        if (!update_scheduled) {
            update_scheduled = true;
            resolved_promise.then(flush);
        }
    }
    function add_render_callback(fn) {
        render_callbacks.push(fn);
    }
    let flushing = false;
    const seen_callbacks = new Set();
    function flush() {
        if (flushing)
            return;
        flushing = true;
        do {
            // first, call beforeUpdate functions
            // and update components
            for (let i = 0; i < dirty_components.length; i += 1) {
                const component = dirty_components[i];
                set_current_component(component);
                update(component.$$);
            }
            set_current_component(null);
            dirty_components.length = 0;
            while (binding_callbacks.length)
                binding_callbacks.pop()();
            // then, once components are updated, call
            // afterUpdate functions. This may cause
            // subsequent updates...
            for (let i = 0; i < render_callbacks.length; i += 1) {
                const callback = render_callbacks[i];
                if (!seen_callbacks.has(callback)) {
                    // ...so guard against infinite loops
                    seen_callbacks.add(callback);
                    callback();
                }
            }
            render_callbacks.length = 0;
        } while (dirty_components.length);
        while (flush_callbacks.length) {
            flush_callbacks.pop()();
        }
        update_scheduled = false;
        flushing = false;
        seen_callbacks.clear();
    }
    function update($$) {
        if ($$.fragment !== null) {
            $$.update();
            run_all($$.before_update);
            const dirty = $$.dirty;
            $$.dirty = [-1];
            $$.fragment && $$.fragment.p($$.ctx, dirty);
            $$.after_update.forEach(add_render_callback);
        }
    }
    const outroing = new Set();
    let outros;
    function group_outros() {
        outros = {
            r: 0,
            c: [],
            p: outros // parent group
        };
    }
    function check_outros() {
        if (!outros.r) {
            run_all(outros.c);
        }
        outros = outros.p;
    }
    function transition_in(block, local) {
        if (block && block.i) {
            outroing.delete(block);
            block.i(local);
        }
    }
    function transition_out(block, local, detach, callback) {
        if (block && block.o) {
            if (outroing.has(block))
                return;
            outroing.add(block);
            outros.c.push(() => {
                outroing.delete(block);
                if (callback) {
                    if (detach)
                        block.d(1);
                    callback();
                }
            });
            block.o(local);
        }
    }

    const globals = (typeof window !== 'undefined'
        ? window
        : typeof globalThis !== 'undefined'
            ? globalThis
            : global);
    function create_component(block) {
        block && block.c();
    }
    function mount_component(component, target, anchor, customElement) {
        const { fragment, on_mount, on_destroy, after_update } = component.$$;
        fragment && fragment.m(target, anchor);
        if (!customElement) {
            // onMount happens before the initial afterUpdate
            add_render_callback(() => {
                const new_on_destroy = on_mount.map(run).filter(is_function);
                if (on_destroy) {
                    on_destroy.push(...new_on_destroy);
                }
                else {
                    // Edge case - component was destroyed immediately,
                    // most likely as a result of a binding initialising
                    run_all(new_on_destroy);
                }
                component.$$.on_mount = [];
            });
        }
        after_update.forEach(add_render_callback);
    }
    function destroy_component(component, detaching) {
        const $$ = component.$$;
        if ($$.fragment !== null) {
            run_all($$.on_destroy);
            $$.fragment && $$.fragment.d(detaching);
            // TODO null out other refs, including component.$$ (but need to
            // preserve final state?)
            $$.on_destroy = $$.fragment = null;
            $$.ctx = [];
        }
    }
    function make_dirty(component, i) {
        if (component.$$.dirty[0] === -1) {
            dirty_components.push(component);
            schedule_update();
            component.$$.dirty.fill(0);
        }
        component.$$.dirty[(i / 31) | 0] |= (1 << (i % 31));
    }
    function init(component, options, instance, create_fragment, not_equal, props, append_styles, dirty = [-1]) {
        const parent_component = current_component;
        set_current_component(component);
        const $$ = component.$$ = {
            fragment: null,
            ctx: null,
            // state
            props,
            update: noop,
            not_equal,
            bound: blank_object(),
            // lifecycle
            on_mount: [],
            on_destroy: [],
            on_disconnect: [],
            before_update: [],
            after_update: [],
            context: new Map(options.context || (parent_component ? parent_component.$$.context : [])),
            // everything else
            callbacks: blank_object(),
            dirty,
            skip_bound: false,
            root: options.target || parent_component.$$.root
        };
        append_styles && append_styles($$.root);
        let ready = false;
        $$.ctx = instance
            ? instance(component, options.props || {}, (i, ret, ...rest) => {
                const value = rest.length ? rest[0] : ret;
                if ($$.ctx && not_equal($$.ctx[i], $$.ctx[i] = value)) {
                    if (!$$.skip_bound && $$.bound[i])
                        $$.bound[i](value);
                    if (ready)
                        make_dirty(component, i);
                }
                return ret;
            })
            : [];
        $$.update();
        ready = true;
        run_all($$.before_update);
        // `false` as a special case of no DOM component
        $$.fragment = create_fragment ? create_fragment($$.ctx) : false;
        if (options.target) {
            if (options.hydrate) {
                const nodes = children(options.target);
                // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
                $$.fragment && $$.fragment.l(nodes);
                nodes.forEach(detach);
            }
            else {
                // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
                $$.fragment && $$.fragment.c();
            }
            if (options.intro)
                transition_in(component.$$.fragment);
            mount_component(component, options.target, options.anchor, options.customElement);
            flush();
        }
        set_current_component(parent_component);
    }
    /**
     * Base class for Svelte components. Used when dev=false.
     */
    class SvelteComponent {
        $destroy() {
            destroy_component(this, 1);
            this.$destroy = noop;
        }
        $on(type, callback) {
            const callbacks = (this.$$.callbacks[type] || (this.$$.callbacks[type] = []));
            callbacks.push(callback);
            return () => {
                const index = callbacks.indexOf(callback);
                if (index !== -1)
                    callbacks.splice(index, 1);
            };
        }
        $set($$props) {
            if (this.$$set && !is_empty($$props)) {
                this.$$.skip_bound = true;
                this.$$set($$props);
                this.$$.skip_bound = false;
            }
        }
    }

    function dispatch_dev(type, detail) {
        document.dispatchEvent(custom_event(type, Object.assign({ version: '3.44.2' }, detail), true));
    }
    function append_dev(target, node) {
        dispatch_dev('SvelteDOMInsert', { target, node });
        append(target, node);
    }
    function insert_dev(target, node, anchor) {
        dispatch_dev('SvelteDOMInsert', { target, node, anchor });
        insert(target, node, anchor);
    }
    function detach_dev(node) {
        dispatch_dev('SvelteDOMRemove', { node });
        detach(node);
    }
    function listen_dev(node, event, handler, options, has_prevent_default, has_stop_propagation) {
        const modifiers = options === true ? ['capture'] : options ? Array.from(Object.keys(options)) : [];
        if (has_prevent_default)
            modifiers.push('preventDefault');
        if (has_stop_propagation)
            modifiers.push('stopPropagation');
        dispatch_dev('SvelteDOMAddEventListener', { node, event, handler, modifiers });
        const dispose = listen(node, event, handler, options);
        return () => {
            dispatch_dev('SvelteDOMRemoveEventListener', { node, event, handler, modifiers });
            dispose();
        };
    }
    function attr_dev(node, attribute, value) {
        attr(node, attribute, value);
        if (value == null)
            dispatch_dev('SvelteDOMRemoveAttribute', { node, attribute });
        else
            dispatch_dev('SvelteDOMSetAttribute', { node, attribute, value });
    }
    function prop_dev(node, property, value) {
        node[property] = value;
        dispatch_dev('SvelteDOMSetProperty', { node, property, value });
    }
    function set_data_dev(text, data) {
        data = '' + data;
        if (text.wholeText === data)
            return;
        dispatch_dev('SvelteDOMSetData', { node: text, data });
        text.data = data;
    }
    function validate_each_argument(arg) {
        if (typeof arg !== 'string' && !(arg && typeof arg === 'object' && 'length' in arg)) {
            let msg = '{#each} only iterates over array-like objects.';
            if (typeof Symbol === 'function' && arg && Symbol.iterator in arg) {
                msg += ' You can use a spread to convert this iterable into an array.';
            }
            throw new Error(msg);
        }
    }
    function validate_slots(name, slot, keys) {
        for (const slot_key of Object.keys(slot)) {
            if (!~keys.indexOf(slot_key)) {
                console.warn(`<${name}> received an unexpected slot "${slot_key}".`);
            }
        }
    }
    /**
     * Base class for Svelte components with some minor dev-enhancements. Used when dev=true.
     */
    class SvelteComponentDev extends SvelteComponent {
        constructor(options) {
            if (!options || (!options.target && !options.$$inline)) {
                throw new Error("'target' is a required option");
            }
            super();
        }
        $destroy() {
            super.$destroy();
            this.$destroy = () => {
                console.warn('Component was already destroyed'); // eslint-disable-line no-console
            };
        }
        $capture_state() { }
        $inject_state() { }
    }

    /* src/MqttForm.svelte generated by Svelte v3.44.2 */

    const file$5 = "src/MqttForm.svelte";

    function create_fragment$6(ctx) {
    	let fieldset;
    	let legend;
    	let t1;
    	let div0;
    	let label0;
    	let t3;
    	let input0;
    	let t4;
    	let div1;
    	let label1;
    	let t6;
    	let input1;
    	let t7;
    	let div2;
    	let label2;
    	let t9;
    	let input2;
    	let t10;
    	let div3;
    	let label3;
    	let t12;
    	let input3;
    	let t13;
    	let div4;
    	let label4;
    	let t15;
    	let input4;
    	let br;

    	const block = {
    		c: function create() {
    			fieldset = element("fieldset");
    			legend = element("legend");
    			legend.textContent = "MQTT Settings";
    			t1 = space();
    			div0 = element("div");
    			label0 = element("label");
    			label0.textContent = "Hostname or IP Address";
    			t3 = space();
    			input0 = element("input");
    			t4 = space();
    			div1 = element("div");
    			label1 = element("label");
    			label1.textContent = "Port number";
    			t6 = space();
    			input1 = element("input");
    			t7 = space();
    			div2 = element("div");
    			label2 = element("label");
    			label2.textContent = "Username";
    			t9 = space();
    			input2 = element("input");
    			t10 = space();
    			div3 = element("div");
    			label3 = element("label");
    			label3.textContent = "Password";
    			t12 = space();
    			input3 = element("input");
    			t13 = space();
    			div4 = element("div");
    			label4 = element("label");
    			label4.textContent = "ID";
    			t15 = space();
    			input4 = element("input");
    			br = element("br");
    			add_location(legend, file$5, 1, 2, 13);
    			attr_dev(label0, "for", "mqtt_host");
    			add_location(label0, file$5, 3, 4, 75);
    			attr_dev(input0, "id", "mqtt_host");
    			add_location(input0, file$5, 4, 4, 133);
    			attr_dev(div0, "class", "form-group");
    			add_location(div0, file$5, 2, 2, 46);
    			attr_dev(label1, "for", "mqtt_port");
    			add_location(label1, file$5, 7, 4, 197);
    			attr_dev(input1, "id", "mqtt_port");
    			attr_dev(input1, "type", "number");
    			attr_dev(input1, "min", "0");
    			attr_dev(input1, "max", "65536");
    			add_location(input1, file$5, 8, 4, 244);
    			attr_dev(div1, "class", "form-group");
    			add_location(div1, file$5, 6, 2, 168);
    			attr_dev(label2, "for", "mqtt_username");
    			add_location(label2, file$5, 11, 4, 342);
    			attr_dev(input2, "id", "mqtt_username");
    			add_location(input2, file$5, 12, 4, 390);
    			attr_dev(div2, "class", "form-group");
    			add_location(div2, file$5, 10, 2, 313);
    			attr_dev(label3, "id", "mqtt_password");
    			add_location(label3, file$5, 15, 4, 458);
    			attr_dev(input3, "id", "mqtt_password");
    			add_location(input3, file$5, 16, 4, 505);
    			attr_dev(div3, "class", "form-group");
    			add_location(div3, file$5, 14, 2, 429);
    			attr_dev(label4, "for", "mqtt_id");
    			add_location(label4, file$5, 19, 4, 573);
    			attr_dev(input4, "id", "mqtt_id");
    			add_location(input4, file$5, 20, 4, 609);
    			add_location(br, file$5, 20, 26, 631);
    			attr_dev(div4, "class", "form-group");
    			add_location(div4, file$5, 18, 2, 544);
    			add_location(fieldset, file$5, 0, 0, 0);
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, fieldset, anchor);
    			append_dev(fieldset, legend);
    			append_dev(fieldset, t1);
    			append_dev(fieldset, div0);
    			append_dev(div0, label0);
    			append_dev(div0, t3);
    			append_dev(div0, input0);
    			append_dev(fieldset, t4);
    			append_dev(fieldset, div1);
    			append_dev(div1, label1);
    			append_dev(div1, t6);
    			append_dev(div1, input1);
    			append_dev(fieldset, t7);
    			append_dev(fieldset, div2);
    			append_dev(div2, label2);
    			append_dev(div2, t9);
    			append_dev(div2, input2);
    			append_dev(fieldset, t10);
    			append_dev(fieldset, div3);
    			append_dev(div3, label3);
    			append_dev(div3, t12);
    			append_dev(div3, input3);
    			append_dev(fieldset, t13);
    			append_dev(fieldset, div4);
    			append_dev(div4, label4);
    			append_dev(div4, t15);
    			append_dev(div4, input4);
    			append_dev(div4, br);
    		},
    		p: noop,
    		i: noop,
    		o: noop,
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(fieldset);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment$6.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance$6($$self, $$props) {
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('MqttForm', slots, []);
    	const writable_props = [];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console.warn(`<MqttForm> was created with unknown prop '${key}'`);
    	});

    	return [];
    }

    class MqttForm extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance$6, create_fragment$6, safe_not_equal, {});

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "MqttForm",
    			options,
    			id: create_fragment$6.name
    		});
    	}
    }

    const subscriber_queue = [];
    /**
     * Create a `Writable` store that allows both updating and reading by subscription.
     * @param {*=}value initial value
     * @param {StartStopNotifier=}start start and stop notifications for subscriptions
     */
    function writable(value, start = noop) {
        let stop;
        const subscribers = new Set();
        function set(new_value) {
            if (safe_not_equal(value, new_value)) {
                value = new_value;
                if (stop) { // store is ready
                    const run_queue = !subscriber_queue.length;
                    for (const subscriber of subscribers) {
                        subscriber[1]();
                        subscriber_queue.push(subscriber, value);
                    }
                    if (run_queue) {
                        for (let i = 0; i < subscriber_queue.length; i += 2) {
                            subscriber_queue[i][0](subscriber_queue[i + 1]);
                        }
                        subscriber_queue.length = 0;
                    }
                }
            }
        }
        function update(fn) {
            set(fn(value));
        }
        function subscribe(run, invalidate = noop) {
            const subscriber = [run, invalidate];
            subscribers.add(subscriber);
            if (subscribers.size === 1) {
                stop = start(set) || noop;
            }
            run(value);
            return () => {
                subscribers.delete(subscriber);
                if (subscribers.size === 0) {
                    stop();
                    stop = null;
                }
            };
        }
        return { set, update, subscribe };
    }

    const networks = writable([]);
    const loading_networks = writable(false);

    const wifi_ssid = writable('');
    const wifi_password = writable('');

    const restarted = writable(false);

    /* src/Loader.svelte generated by Svelte v3.44.2 */

    const file$4 = "src/Loader.svelte";

    function create_fragment$5(ctx) {
    	let div9;
    	let div0;
    	let div1;
    	let div2;
    	let div3;
    	let div4;
    	let div5;
    	let div6;
    	let div7;
    	let div8;

    	const block = {
    		c: function create() {
    			div9 = element("div");
    			div0 = element("div");
    			div1 = element("div");
    			div2 = element("div");
    			div3 = element("div");
    			div4 = element("div");
    			div5 = element("div");
    			div6 = element("div");
    			div7 = element("div");
    			div8 = element("div");
    			attr_dev(div0, "class", "svelte-1vdom7f");
    			add_location(div0, file$4, 0, 22, 22);
    			attr_dev(div1, "class", "svelte-1vdom7f");
    			add_location(div1, file$4, 0, 33, 33);
    			attr_dev(div2, "class", "svelte-1vdom7f");
    			add_location(div2, file$4, 0, 44, 44);
    			attr_dev(div3, "class", "svelte-1vdom7f");
    			add_location(div3, file$4, 0, 55, 55);
    			attr_dev(div4, "class", "svelte-1vdom7f");
    			add_location(div4, file$4, 0, 66, 66);
    			attr_dev(div5, "class", "svelte-1vdom7f");
    			add_location(div5, file$4, 0, 77, 77);
    			attr_dev(div6, "class", "svelte-1vdom7f");
    			add_location(div6, file$4, 0, 88, 88);
    			attr_dev(div7, "class", "svelte-1vdom7f");
    			add_location(div7, file$4, 0, 99, 99);
    			attr_dev(div8, "class", "svelte-1vdom7f");
    			add_location(div8, file$4, 0, 110, 110);
    			attr_dev(div9, "class", "lds-grid svelte-1vdom7f");
    			add_location(div9, file$4, 0, 0, 0);
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, div9, anchor);
    			append_dev(div9, div0);
    			append_dev(div9, div1);
    			append_dev(div9, div2);
    			append_dev(div9, div3);
    			append_dev(div9, div4);
    			append_dev(div9, div5);
    			append_dev(div9, div6);
    			append_dev(div9, div7);
    			append_dev(div9, div8);
    		},
    		p: noop,
    		i: noop,
    		o: noop,
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(div9);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment$5.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance$5($$self, $$props) {
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('Loader', slots, []);
    	const writable_props = [];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console.warn(`<Loader> was created with unknown prop '${key}'`);
    	});

    	return [];
    }

    class Loader extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance$5, create_fragment$5, safe_not_equal, {});

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "Loader",
    			options,
    			id: create_fragment$5.name
    		});
    	}
    }

    /* src/WiFiList.svelte generated by Svelte v3.44.2 */
    const file$3 = "src/WiFiList.svelte";

    function get_each_context(ctx, list, i) {
    	const child_ctx = ctx.slice();
    	child_ctx[8] = list[i];
    	return child_ctx;
    }

    // (36:0) {:else}
    function create_else_block_1(ctx) {
    	let loader;
    	let t0;
    	let p;
    	let current;
    	loader = new Loader({ $$inline: true });

    	const block = {
    		c: function create() {
    			create_component(loader.$$.fragment);
    			t0 = space();
    			p = element("p");
    			p.textContent = "Loading networks ...";
    			add_location(p, file$3, 37, 0, 872);
    		},
    		m: function mount(target, anchor) {
    			mount_component(loader, target, anchor);
    			insert_dev(target, t0, anchor);
    			insert_dev(target, p, anchor);
    			current = true;
    		},
    		p: noop,
    		i: function intro(local) {
    			if (current) return;
    			transition_in(loader.$$.fragment, local);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(loader.$$.fragment, local);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			destroy_component(loader, detaching);
    			if (detaching) detach_dev(t0);
    			if (detaching) detach_dev(p);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_else_block_1.name,
    		type: "else",
    		source: "(36:0) {:else}",
    		ctx
    	});

    	return block;
    }

    // (26:0) {#if !isLoading}
    function create_if_block$1(ctx) {
    	let if_block_anchor;

    	function select_block_type_1(ctx, dirty) {
    		if (/*networksList*/ ctx[1].length == 0) return create_if_block_1;
    		return create_else_block$1;
    	}

    	let current_block_type = select_block_type_1(ctx);
    	let if_block = current_block_type(ctx);

    	const block = {
    		c: function create() {
    			if_block.c();
    			if_block_anchor = empty();
    		},
    		m: function mount(target, anchor) {
    			if_block.m(target, anchor);
    			insert_dev(target, if_block_anchor, anchor);
    		},
    		p: function update(ctx, dirty) {
    			if (current_block_type === (current_block_type = select_block_type_1(ctx)) && if_block) {
    				if_block.p(ctx, dirty);
    			} else {
    				if_block.d(1);
    				if_block = current_block_type(ctx);

    				if (if_block) {
    					if_block.c();
    					if_block.m(if_block_anchor.parentNode, if_block_anchor);
    				}
    			}
    		},
    		i: noop,
    		o: noop,
    		d: function destroy(detaching) {
    			if_block.d(detaching);
    			if (detaching) detach_dev(if_block_anchor);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_if_block$1.name,
    		type: "if",
    		source: "(26:0) {#if !isLoading}",
    		ctx
    	});

    	return block;
    }

    // (29:2) {:else}
    function create_else_block$1(ctx) {
    	let ul;
    	let each_value = /*$networks*/ ctx[2];
    	validate_each_argument(each_value);
    	let each_blocks = [];

    	for (let i = 0; i < each_value.length; i += 1) {
    		each_blocks[i] = create_each_block(get_each_context(ctx, each_value, i));
    	}

    	const block = {
    		c: function create() {
    			ul = element("ul");

    			for (let i = 0; i < each_blocks.length; i += 1) {
    				each_blocks[i].c();
    			}

    			attr_dev(ul, "class", "wifi-networks-list svelte-1pkh0sr");
    			add_location(ul, file$3, 29, 4, 683);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, ul, anchor);

    			for (let i = 0; i < each_blocks.length; i += 1) {
    				each_blocks[i].m(ul, null);
    			}
    		},
    		p: function update(ctx, dirty) {
    			if (dirty & /*selectNetwork, $networks*/ 12) {
    				each_value = /*$networks*/ ctx[2];
    				validate_each_argument(each_value);
    				let i;

    				for (i = 0; i < each_value.length; i += 1) {
    					const child_ctx = get_each_context(ctx, each_value, i);

    					if (each_blocks[i]) {
    						each_blocks[i].p(child_ctx, dirty);
    					} else {
    						each_blocks[i] = create_each_block(child_ctx);
    						each_blocks[i].c();
    						each_blocks[i].m(ul, null);
    					}
    				}

    				for (; i < each_blocks.length; i += 1) {
    					each_blocks[i].d(1);
    				}

    				each_blocks.length = each_value.length;
    			}
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(ul);
    			destroy_each(each_blocks, detaching);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_else_block$1.name,
    		type: "else",
    		source: "(29:2) {:else}",
    		ctx
    	});

    	return block;
    }

    // (27:2) {#if networksList.length == 0}
    function create_if_block_1(ctx) {
    	let p;

    	const block = {
    		c: function create() {
    			p = element("p");
    			p.textContent = "No wifi networks available.";
    			add_location(p, file$3, 27, 4, 634);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, p, anchor);
    		},
    		p: noop,
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(p);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_if_block_1.name,
    		type: "if",
    		source: "(27:2) {#if networksList.length == 0}",
    		ctx
    	});

    	return block;
    }

    // (31:6) {#each $networks as network}
    function create_each_block(ctx) {
    	let li;
    	let t_value = /*network*/ ctx[8].ssid + "";
    	let t;
    	let mounted;
    	let dispose;

    	function click_handler() {
    		return /*click_handler*/ ctx[4](/*network*/ ctx[8]);
    	}

    	const block = {
    		c: function create() {
    			li = element("li");
    			t = text(t_value);
    			attr_dev(li, "class", "svelte-1pkh0sr");
    			add_location(li, file$3, 31, 6, 756);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, li, anchor);
    			append_dev(li, t);

    			if (!mounted) {
    				dispose = listen_dev(li, "click", click_handler, false, false, false);
    				mounted = true;
    			}
    		},
    		p: function update(new_ctx, dirty) {
    			ctx = new_ctx;
    			if (dirty & /*$networks*/ 4 && t_value !== (t_value = /*network*/ ctx[8].ssid + "")) set_data_dev(t, t_value);
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(li);
    			mounted = false;
    			dispose();
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_each_block.name,
    		type: "each",
    		source: "(31:6) {#each $networks as network}",
    		ctx
    	});

    	return block;
    }

    function create_fragment$4(ctx) {
    	let current_block_type_index;
    	let if_block;
    	let if_block_anchor;
    	let current;
    	const if_block_creators = [create_if_block$1, create_else_block_1];
    	const if_blocks = [];

    	function select_block_type(ctx, dirty) {
    		if (!/*isLoading*/ ctx[0]) return 0;
    		return 1;
    	}

    	current_block_type_index = select_block_type(ctx);
    	if_block = if_blocks[current_block_type_index] = if_block_creators[current_block_type_index](ctx);

    	const block = {
    		c: function create() {
    			if_block.c();
    			if_block_anchor = empty();
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			if_blocks[current_block_type_index].m(target, anchor);
    			insert_dev(target, if_block_anchor, anchor);
    			current = true;
    		},
    		p: function update(ctx, [dirty]) {
    			let previous_block_index = current_block_type_index;
    			current_block_type_index = select_block_type(ctx);

    			if (current_block_type_index === previous_block_index) {
    				if_blocks[current_block_type_index].p(ctx, dirty);
    			} else {
    				group_outros();

    				transition_out(if_blocks[previous_block_index], 1, 1, () => {
    					if_blocks[previous_block_index] = null;
    				});

    				check_outros();
    				if_block = if_blocks[current_block_type_index];

    				if (!if_block) {
    					if_block = if_blocks[current_block_type_index] = if_block_creators[current_block_type_index](ctx);
    					if_block.c();
    				} else {
    					if_block.p(ctx, dirty);
    				}

    				transition_in(if_block, 1);
    				if_block.m(if_block_anchor.parentNode, if_block_anchor);
    			}
    		},
    		i: function intro(local) {
    			if (current) return;
    			transition_in(if_block);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(if_block);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			if_blocks[current_block_type_index].d(detaching);
    			if (detaching) detach_dev(if_block_anchor);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment$4.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance$4($$self, $$props, $$invalidate) {
    	let $networks;
    	validate_store(networks, 'networks');
    	component_subscribe($$self, networks, $$value => $$invalidate(2, $networks = $$value));
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('WiFiList', slots, []);
    	const dispatch = createEventDispatcher();
    	let isLoading = false;
    	let networksList = [];

    	const unsub_loading = loading_networks.subscribe(value => {
    		$$invalidate(0, isLoading = value);
    	});

    	const unsub_networks = networks.subscribe(value => $$invalidate(1, networksList = value));

    	onDestroy(() => {
    		unsub_loading();
    		unsub_networks();
    	});

    	function selectNetwork(network) {
    		dispatch('network', network);
    	}

    	const writable_props = [];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console.warn(`<WiFiList> was created with unknown prop '${key}'`);
    	});

    	const click_handler = network => selectNetwork(network);

    	$$self.$capture_state = () => ({
    		onDestroy,
    		createEventDispatcher,
    		loading_networks,
    		networks,
    		wifi_password,
    		wifi_ssid,
    		Loader,
    		dispatch,
    		isLoading,
    		networksList,
    		unsub_loading,
    		unsub_networks,
    		selectNetwork,
    		$networks
    	});

    	$$self.$inject_state = $$props => {
    		if ('isLoading' in $$props) $$invalidate(0, isLoading = $$props.isLoading);
    		if ('networksList' in $$props) $$invalidate(1, networksList = $$props.networksList);
    	};

    	if ($$props && "$$inject" in $$props) {
    		$$self.$inject_state($$props.$$inject);
    	}

    	return [isLoading, networksList, $networks, selectNetwork, click_handler];
    }

    class WiFiList extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance$4, create_fragment$4, safe_not_equal, {});

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "WiFiList",
    			options,
    			id: create_fragment$4.name
    		});
    	}
    }

    /* src/Modal.svelte generated by Svelte v3.44.2 */

    const { console: console_1 } = globals;
    const file$2 = "src/Modal.svelte";
    const get_modal_footer_slot_changes = dirty => ({});
    const get_modal_footer_slot_context = ctx => ({});
    const get_modal_header_slot_changes = dirty => ({});
    const get_modal_header_slot_context = ctx => ({});

    // (43:32)          
    function fallback_block_1(ctx) {
    	let h1;
    	let t;

    	const block = {
    		c: function create() {
    			h1 = element("h1");
    			t = text(/*title*/ ctx[0]);
    			add_location(h1, file$2, 43, 8, 863);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, h1, anchor);
    			append_dev(h1, t);
    		},
    		p: function update(ctx, dirty) {
    			if (dirty & /*title*/ 1) set_data_dev(t, /*title*/ ctx[0]);
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(h1);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: fallback_block_1.name,
    		type: "fallback",
    		source: "(43:32)          ",
    		ctx
    	});

    	return block;
    }

    // (51:32)          
    function fallback_block(ctx) {
    	let button;
    	let t;
    	let button_id_value;
    	let mounted;
    	let dispose;

    	const block = {
    		c: function create() {
    			button = element("button");
    			t = text("Close");
    			attr_dev(button, "type", "button");
    			attr_dev(button, "id", button_id_value = /*id*/ ctx[1] + '-close-button');
    			add_location(button, file$2, 51, 8, 1040);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, button, anchor);
    			append_dev(button, t);

    			if (!mounted) {
    				dispose = listen_dev(button, "click", /*closeButton*/ ctx[2], false, false, false);
    				mounted = true;
    			}
    		},
    		p: function update(ctx, dirty) {
    			if (dirty & /*id*/ 2 && button_id_value !== (button_id_value = /*id*/ ctx[1] + '-close-button')) {
    				attr_dev(button, "id", button_id_value);
    			}
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(button);
    			mounted = false;
    			dispose();
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: fallback_block.name,
    		type: "fallback",
    		source: "(51:32)          ",
    		ctx
    	});

    	return block;
    }

    function create_fragment$3(ctx) {
    	let div4;
    	let div3;
    	let div0;
    	let t0;
    	let div1;
    	let t1;
    	let div2;
    	let current;
    	const modal_header_slot_template = /*#slots*/ ctx[5]["modal-header"];
    	const modal_header_slot = create_slot(modal_header_slot_template, ctx, /*$$scope*/ ctx[4], get_modal_header_slot_context);
    	const modal_header_slot_or_fallback = modal_header_slot || fallback_block_1(ctx);
    	const default_slot_template = /*#slots*/ ctx[5].default;
    	const default_slot = create_slot(default_slot_template, ctx, /*$$scope*/ ctx[4], null);
    	const modal_footer_slot_template = /*#slots*/ ctx[5]["modal-footer"];
    	const modal_footer_slot = create_slot(modal_footer_slot_template, ctx, /*$$scope*/ ctx[4], get_modal_footer_slot_context);
    	const modal_footer_slot_or_fallback = modal_footer_slot || fallback_block(ctx);

    	const block = {
    		c: function create() {
    			div4 = element("div");
    			div3 = element("div");
    			div0 = element("div");
    			if (modal_header_slot_or_fallback) modal_header_slot_or_fallback.c();
    			t0 = space();
    			div1 = element("div");
    			if (default_slot) default_slot.c();
    			t1 = space();
    			div2 = element("div");
    			if (modal_footer_slot_or_fallback) modal_footer_slot_or_fallback.c();
    			attr_dev(div0, "class", "modal-header svelte-1co589i");
    			add_location(div0, file$2, 41, 4, 795);
    			attr_dev(div1, "class", "modal-content svelte-1co589i");
    			add_location(div1, file$2, 46, 4, 909);
    			attr_dev(div2, "class", "modal-footer svelte-1co589i");
    			add_location(div2, file$2, 49, 4, 972);
    			attr_dev(div3, "class", "modal-body svelte-1co589i");
    			add_location(div3, file$2, 40, 2, 766);
    			attr_dev(div4, "class", "modal svelte-1co589i");
    			attr_dev(div4, "id", /*id*/ ctx[1]);
    			add_location(div4, file$2, 39, 0, 736);
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, div4, anchor);
    			append_dev(div4, div3);
    			append_dev(div3, div0);

    			if (modal_header_slot_or_fallback) {
    				modal_header_slot_or_fallback.m(div0, null);
    			}

    			append_dev(div3, t0);
    			append_dev(div3, div1);

    			if (default_slot) {
    				default_slot.m(div1, null);
    			}

    			append_dev(div3, t1);
    			append_dev(div3, div2);

    			if (modal_footer_slot_or_fallback) {
    				modal_footer_slot_or_fallback.m(div2, null);
    			}

    			current = true;
    		},
    		p: function update(ctx, [dirty]) {
    			if (modal_header_slot) {
    				if (modal_header_slot.p && (!current || dirty & /*$$scope*/ 16)) {
    					update_slot_base(
    						modal_header_slot,
    						modal_header_slot_template,
    						ctx,
    						/*$$scope*/ ctx[4],
    						!current
    						? get_all_dirty_from_scope(/*$$scope*/ ctx[4])
    						: get_slot_changes(modal_header_slot_template, /*$$scope*/ ctx[4], dirty, get_modal_header_slot_changes),
    						get_modal_header_slot_context
    					);
    				}
    			} else {
    				if (modal_header_slot_or_fallback && modal_header_slot_or_fallback.p && (!current || dirty & /*title*/ 1)) {
    					modal_header_slot_or_fallback.p(ctx, !current ? -1 : dirty);
    				}
    			}

    			if (default_slot) {
    				if (default_slot.p && (!current || dirty & /*$$scope*/ 16)) {
    					update_slot_base(
    						default_slot,
    						default_slot_template,
    						ctx,
    						/*$$scope*/ ctx[4],
    						!current
    						? get_all_dirty_from_scope(/*$$scope*/ ctx[4])
    						: get_slot_changes(default_slot_template, /*$$scope*/ ctx[4], dirty, null),
    						null
    					);
    				}
    			}

    			if (modal_footer_slot) {
    				if (modal_footer_slot.p && (!current || dirty & /*$$scope*/ 16)) {
    					update_slot_base(
    						modal_footer_slot,
    						modal_footer_slot_template,
    						ctx,
    						/*$$scope*/ ctx[4],
    						!current
    						? get_all_dirty_from_scope(/*$$scope*/ ctx[4])
    						: get_slot_changes(modal_footer_slot_template, /*$$scope*/ ctx[4], dirty, get_modal_footer_slot_changes),
    						get_modal_footer_slot_context
    					);
    				}
    			} else {
    				if (modal_footer_slot_or_fallback && modal_footer_slot_or_fallback.p && (!current || dirty & /*id*/ 2)) {
    					modal_footer_slot_or_fallback.p(ctx, !current ? -1 : dirty);
    				}
    			}

    			if (!current || dirty & /*id*/ 2) {
    				attr_dev(div4, "id", /*id*/ ctx[1]);
    			}
    		},
    		i: function intro(local) {
    			if (current) return;
    			transition_in(modal_header_slot_or_fallback, local);
    			transition_in(default_slot, local);
    			transition_in(modal_footer_slot_or_fallback, local);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(modal_header_slot_or_fallback, local);
    			transition_out(default_slot, local);
    			transition_out(modal_footer_slot_or_fallback, local);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(div4);
    			if (modal_header_slot_or_fallback) modal_header_slot_or_fallback.d(detaching);
    			if (default_slot) default_slot.d(detaching);
    			if (modal_footer_slot_or_fallback) modal_footer_slot_or_fallback.d(detaching);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment$3.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance$3($$self, $$props, $$invalidate) {
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('Modal', slots, ['modal-header','default','modal-footer']);
    	let { title = '' } = $$props;
    	let { open = false } = $$props;
    	let { id = '' } = $$props;
    	const dispatch = createEventDispatcher();

    	function hideModal() {
    		const el = document.getElementById(id);

    		if (!el) {
    			console.warn('Modal for id ' + id + ' does not exist');
    		} else {
    			el.style.display = 'none';
    		}

    		dispatch('closed', {});
    	}

    	function showModal() {
    		const el = document.getElementById(id);

    		if (!el) {
    			console.warn('Modal for id ' + id + ' does not exist');
    		} else {
    			el.style.display = 'block';
    		}

    		dispatch('shown', {});
    	}

    	function closeButton() {
    		$$invalidate(3, open = false);
    	}

    	afterUpdate(() => {
    		open ? showModal() : hideModal();
    	});

    	const writable_props = ['title', 'open', 'id'];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console_1.warn(`<Modal> was created with unknown prop '${key}'`);
    	});

    	$$self.$$set = $$props => {
    		if ('title' in $$props) $$invalidate(0, title = $$props.title);
    		if ('open' in $$props) $$invalidate(3, open = $$props.open);
    		if ('id' in $$props) $$invalidate(1, id = $$props.id);
    		if ('$$scope' in $$props) $$invalidate(4, $$scope = $$props.$$scope);
    	};

    	$$self.$capture_state = () => ({
    		onMount,
    		createEventDispatcher,
    		afterUpdate,
    		title,
    		open,
    		id,
    		dispatch,
    		hideModal,
    		showModal,
    		closeButton
    	});

    	$$self.$inject_state = $$props => {
    		if ('title' in $$props) $$invalidate(0, title = $$props.title);
    		if ('open' in $$props) $$invalidate(3, open = $$props.open);
    		if ('id' in $$props) $$invalidate(1, id = $$props.id);
    	};

    	if ($$props && "$$inject" in $$props) {
    		$$self.$inject_state($$props.$$inject);
    	}

    	return [title, id, closeButton, open, $$scope, slots];
    }

    class Modal extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance$3, create_fragment$3, safe_not_equal, { title: 0, open: 3, id: 1 });

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "Modal",
    			options,
    			id: create_fragment$3.name
    		});
    	}

    	get title() {
    		throw new Error("<Modal>: Props cannot be read directly from the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}

    	set title(value) {
    		throw new Error("<Modal>: Props cannot be set directly on the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}

    	get open() {
    		throw new Error("<Modal>: Props cannot be read directly from the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}

    	set open(value) {
    		throw new Error("<Modal>: Props cannot be set directly on the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}

    	get id() {
    		throw new Error("<Modal>: Props cannot be read directly from the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}

    	set id(value) {
    		throw new Error("<Modal>: Props cannot be set directly on the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}
    }

    const url = 'http://192.168.1.230' ;

    async function sleep(amt) {
      return new Promise((resolve) => {
        setTimeout(() => resolve(), amt);
      });
    }
    async function getAvailableWifiNetworks() {
      let data = null;
      while (true) {
        try {
          const response = await fetch(url + '/available-networks');
          if (response.status == 202 || response.status == 204) {
            await sleep(5000);
          } else {
            data = await response.json();
            break;
          }
        } catch (err) {
          console.error(err);
          await sleep(5000);
        }
      }
      return data;
    }

    async function getWifiSettings() {
      const response = await fetch(url + '/wifi-settings');

      const data = await response.json();
      return data;
    }

    async function saveWifiSettings(ssid, pw) {
      const formData = new URLSearchParams({
        'wifi_ssid': ssid,
        'wifi_password': pw
      });

      await fetch(url + '/wifi-settings', {
        method: 'POST',
        body: formData,
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
        }
      });
    }

    async function restart() {
      await fetch(url + '/restart', {
        method: 'POST'
      });
    }

    /* src/WifiModal.svelte generated by Svelte v3.44.2 */

    // (33:0) <Modal id="wifi-modal" on:closed={() => dispatch('closed')} on:shown={onShown} open={open} title="WiFi Networks">
    function create_default_slot(ctx) {
    	let wifilist;
    	let current;
    	wifilist = new WiFiList({ $$inline: true });
    	wifilist.$on("network", /*networkSelected*/ ctx[3]);

    	const block = {
    		c: function create() {
    			create_component(wifilist.$$.fragment);
    		},
    		m: function mount(target, anchor) {
    			mount_component(wifilist, target, anchor);
    			current = true;
    		},
    		p: noop,
    		i: function intro(local) {
    			if (current) return;
    			transition_in(wifilist.$$.fragment, local);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(wifilist.$$.fragment, local);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			destroy_component(wifilist, detaching);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_default_slot.name,
    		type: "slot",
    		source: "(33:0) <Modal id=\\\"wifi-modal\\\" on:closed={() => dispatch('closed')} on:shown={onShown} open={open} title=\\\"WiFi Networks\\\">",
    		ctx
    	});

    	return block;
    }

    function create_fragment$2(ctx) {
    	let modal;
    	let current;

    	modal = new Modal({
    			props: {
    				id: "wifi-modal",
    				open: /*open*/ ctx[0],
    				title: "WiFi Networks",
    				$$slots: { default: [create_default_slot] },
    				$$scope: { ctx }
    			},
    			$$inline: true
    		});

    	modal.$on("closed", /*closed_handler*/ ctx[4]);
    	modal.$on("shown", /*onShown*/ ctx[2]);

    	const block = {
    		c: function create() {
    			create_component(modal.$$.fragment);
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			mount_component(modal, target, anchor);
    			current = true;
    		},
    		p: function update(ctx, [dirty]) {
    			const modal_changes = {};
    			if (dirty & /*open*/ 1) modal_changes.open = /*open*/ ctx[0];

    			if (dirty & /*$$scope*/ 128) {
    				modal_changes.$$scope = { dirty, ctx };
    			}

    			modal.$set(modal_changes);
    		},
    		i: function intro(local) {
    			if (current) return;
    			transition_in(modal.$$.fragment, local);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(modal.$$.fragment, local);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			destroy_component(modal, detaching);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment$2.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance$2($$self, $$props, $$invalidate) {
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('WifiModal', slots, []);
    	let { open = false } = $$props;
    	const dispatch = createEventDispatcher();
    	let showHiddenNetworks = false;

    	async function loadWifiNetworks() {
    		loading_networks.set(true);
    		let data = await getAvailableWifiNetworks();
    		data = data.filter(d => showHiddenNetworks ? true : d.ssid != '');
    		networks.set(data);
    		loading_networks.set(false);
    	}

    	function onShown() {
    		dispatch('shown');
    		loadWifiNetworks();
    	}

    	function networkSelected($e) {
    		wifi_ssid.set($e.detail.ssid);
    		dispatch('closed');
    	}

    	const writable_props = ['open'];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console.warn(`<WifiModal> was created with unknown prop '${key}'`);
    	});

    	const closed_handler = () => dispatch('closed');

    	$$self.$$set = $$props => {
    		if ('open' in $$props) $$invalidate(0, open = $$props.open);
    	};

    	$$self.$capture_state = () => ({
    		open,
    		Modal,
    		createEventDispatcher,
    		WiFiList,
    		loading_networks,
    		networks,
    		wifi_ssid,
    		getAvailableWifiNetworks,
    		dispatch,
    		showHiddenNetworks,
    		loadWifiNetworks,
    		onShown,
    		networkSelected
    	});

    	$$self.$inject_state = $$props => {
    		if ('open' in $$props) $$invalidate(0, open = $$props.open);
    		if ('showHiddenNetworks' in $$props) showHiddenNetworks = $$props.showHiddenNetworks;
    	};

    	if ($$props && "$$inject" in $$props) {
    		$$self.$inject_state($$props.$$inject);
    	}

    	return [open, dispatch, onShown, networkSelected, closed_handler];
    }

    class WifiModal extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance$2, create_fragment$2, safe_not_equal, { open: 0 });

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "WifiModal",
    			options,
    			id: create_fragment$2.name
    		});
    	}

    	get open() {
    		throw new Error("<WifiModal>: Props cannot be read directly from the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}

    	set open(value) {
    		throw new Error("<WifiModal>: Props cannot be set directly on the component instance unless compiling with 'accessors: true' or '<svelte:options accessors/>'");
    	}
    }

    /* src/WiFiForm.svelte generated by Svelte v3.44.2 */
    const file$1 = "src/WiFiForm.svelte";

    function create_fragment$1(ctx) {
    	let wifimodal;
    	let t0;
    	let form;
    	let fieldset;
    	let legend;
    	let t2;
    	let div0;
    	let label0;
    	let t4;
    	let input0;
    	let t5;
    	let div1;
    	let label1;
    	let t7;
    	let input1;
    	let t8;
    	let button0;
    	let t10;
    	let input2;
    	let t11;
    	let button1;
    	let t12_value = (/*loading*/ ctx[1] ? 'Loading' : 'Save') + "";
    	let t12;
    	let current;
    	let mounted;
    	let dispose;

    	wifimodal = new WifiModal({
    			props: { open: /*showWiFiModal*/ ctx[0] },
    			$$inline: true
    		});

    	wifimodal.$on("closed", /*onModalClosed*/ ctx[6]);

    	const block = {
    		c: function create() {
    			create_component(wifimodal.$$.fragment);
    			t0 = space();
    			form = element("form");
    			fieldset = element("fieldset");
    			legend = element("legend");
    			legend.textContent = "WiFi Settings";
    			t2 = space();
    			div0 = element("div");
    			label0 = element("label");
    			label0.textContent = "SSID";
    			t4 = space();
    			input0 = element("input");
    			t5 = space();
    			div1 = element("div");
    			label1 = element("label");
    			label1.textContent = "Password";
    			t7 = space();
    			input1 = element("input");
    			t8 = space();
    			button0 = element("button");
    			button0.textContent = "Scan";
    			t10 = space();
    			input2 = element("input");
    			t11 = space();
    			button1 = element("button");
    			t12 = text(t12_value);
    			add_location(legend, file$1, 34, 2, 810);
    			attr_dev(label0, "for", "wifi_ssid");
    			add_location(label0, file$1, 36, 4, 872);
    			attr_dev(input0, "id", "wifi_ssid");
    			attr_dev(input0, ":disabled", /*loading*/ ctx[1]);
    			add_location(input0, file$1, 37, 4, 912);
    			attr_dev(div0, "class", "form-group");
    			add_location(div0, file$1, 35, 2, 843);
    			attr_dev(label1, "for", "wifi_password");
    			add_location(label1, file$1, 40, 4, 1020);
    			attr_dev(input1, "id", "wifi_password");
    			attr_dev(input1, ":disabled", /*loading*/ ctx[1]);
    			add_location(input1, file$1, 41, 4, 1068);
    			attr_dev(div1, "class", "form-group");
    			add_location(div1, file$1, 39, 2, 991);
    			attr_dev(button0, "id", "scan_wifi");
    			attr_dev(button0, "type", "button");
    			add_location(button0, file$1, 43, 2, 1155);
    			attr_dev(input2, "type", "checkbox");
    			attr_dev(input2, "id", "wifi_include_hidden");
    			add_location(input2, file$1, 44, 2, 1231);
    			add_location(fieldset, file$1, 33, 0, 797);
    			attr_dev(button1, "type", "submit");
    			button1.disabled = /*loading*/ ctx[1];
    			add_location(button1, file$1, 46, 0, 1292);
    			attr_dev(form, "id", "wifi-form");
    			add_location(form, file$1, 32, 0, 754);
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			mount_component(wifimodal, target, anchor);
    			insert_dev(target, t0, anchor);
    			insert_dev(target, form, anchor);
    			append_dev(form, fieldset);
    			append_dev(fieldset, legend);
    			append_dev(fieldset, t2);
    			append_dev(fieldset, div0);
    			append_dev(div0, label0);
    			append_dev(div0, t4);
    			append_dev(div0, input0);
    			set_input_value(input0, /*$wifi_ssid*/ ctx[3]);
    			append_dev(fieldset, t5);
    			append_dev(fieldset, div1);
    			append_dev(div1, label1);
    			append_dev(div1, t7);
    			append_dev(div1, input1);
    			set_input_value(input1, /*$wifi_password*/ ctx[2]);
    			append_dev(fieldset, t8);
    			append_dev(fieldset, button0);
    			append_dev(fieldset, t10);
    			append_dev(fieldset, input2);
    			append_dev(form, t11);
    			append_dev(form, button1);
    			append_dev(button1, t12);
    			current = true;

    			if (!mounted) {
    				dispose = [
    					listen_dev(input0, "input", /*input0_input_handler*/ ctx[7]),
    					listen_dev(input1, "input", /*input1_input_handler*/ ctx[8]),
    					listen_dev(button0, "click", /*onScanClick*/ ctx[5], false, false, false),
    					listen_dev(form, "submit", /*onSubmit*/ ctx[4], false, false, false)
    				];

    				mounted = true;
    			}
    		},
    		p: function update(ctx, [dirty]) {
    			const wifimodal_changes = {};
    			if (dirty & /*showWiFiModal*/ 1) wifimodal_changes.open = /*showWiFiModal*/ ctx[0];
    			wifimodal.$set(wifimodal_changes);

    			if (!current || dirty & /*loading*/ 2) {
    				attr_dev(input0, ":disabled", /*loading*/ ctx[1]);
    			}

    			if (dirty & /*$wifi_ssid*/ 8 && input0.value !== /*$wifi_ssid*/ ctx[3]) {
    				set_input_value(input0, /*$wifi_ssid*/ ctx[3]);
    			}

    			if (!current || dirty & /*loading*/ 2) {
    				attr_dev(input1, ":disabled", /*loading*/ ctx[1]);
    			}

    			if (dirty & /*$wifi_password*/ 4 && input1.value !== /*$wifi_password*/ ctx[2]) {
    				set_input_value(input1, /*$wifi_password*/ ctx[2]);
    			}

    			if ((!current || dirty & /*loading*/ 2) && t12_value !== (t12_value = (/*loading*/ ctx[1] ? 'Loading' : 'Save') + "")) set_data_dev(t12, t12_value);

    			if (!current || dirty & /*loading*/ 2) {
    				prop_dev(button1, "disabled", /*loading*/ ctx[1]);
    			}
    		},
    		i: function intro(local) {
    			if (current) return;
    			transition_in(wifimodal.$$.fragment, local);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(wifimodal.$$.fragment, local);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			destroy_component(wifimodal, detaching);
    			if (detaching) detach_dev(t0);
    			if (detaching) detach_dev(form);
    			mounted = false;
    			run_all(dispose);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment$1.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance$1($$self, $$props, $$invalidate) {
    	let $wifi_password;
    	let $wifi_ssid;
    	validate_store(wifi_password, 'wifi_password');
    	component_subscribe($$self, wifi_password, $$value => $$invalidate(2, $wifi_password = $$value));
    	validate_store(wifi_ssid, 'wifi_ssid');
    	component_subscribe($$self, wifi_ssid, $$value => $$invalidate(3, $wifi_ssid = $$value));
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('WiFiForm', slots, []);
    	let showWiFiModal = false;
    	let loading = false;

    	async function onSubmit(e) {
    		e.preventDefault();
    		saveWifiSettings($wifi_ssid, $wifi_password);
    	}

    	async function onScanClick() {
    		$$invalidate(0, showWiFiModal = true);
    	}

    	function onModalClosed() {
    		$$invalidate(0, showWiFiModal = false);
    	}

    	onMount(async () => {
    		$$invalidate(1, loading = true);
    		const data = await getWifiSettings();
    		wifi_ssid.set(data.wifi_ssid);
    		$$invalidate(1, loading = false);
    	});

    	const writable_props = [];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console.warn(`<WiFiForm> was created with unknown prop '${key}'`);
    	});

    	function input0_input_handler() {
    		$wifi_ssid = this.value;
    		wifi_ssid.set($wifi_ssid);
    	}

    	function input1_input_handler() {
    		$wifi_password = this.value;
    		wifi_password.set($wifi_password);
    	}

    	$$self.$capture_state = () => ({
    		WiFiList,
    		WiFiModal: WifiModal,
    		loading_networks,
    		networks,
    		wifi_ssid,
    		wifi_password,
    		saveWifiSettings,
    		getWifiSettings,
    		onMount,
    		showWiFiModal,
    		loading,
    		onSubmit,
    		onScanClick,
    		onModalClosed,
    		$wifi_password,
    		$wifi_ssid
    	});

    	$$self.$inject_state = $$props => {
    		if ('showWiFiModal' in $$props) $$invalidate(0, showWiFiModal = $$props.showWiFiModal);
    		if ('loading' in $$props) $$invalidate(1, loading = $$props.loading);
    	};

    	if ($$props && "$$inject" in $$props) {
    		$$self.$inject_state($$props.$$inject);
    	}

    	return [
    		showWiFiModal,
    		loading,
    		$wifi_password,
    		$wifi_ssid,
    		onSubmit,
    		onScanClick,
    		onModalClosed,
    		input0_input_handler,
    		input1_input_handler
    	];
    }

    class WiFiForm extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance$1, create_fragment$1, safe_not_equal, {});

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "WiFiForm",
    			options,
    			id: create_fragment$1.name
    		});
    	}
    }

    /* src/App.svelte generated by Svelte v3.44.2 */
    const file = "src/App.svelte";

    // (24:2) {:else}
    function create_else_block(ctx) {
    	let section;
    	let div;
    	let button;
    	let t1;
    	let form;
    	let wifiform;
    	let t2;
    	let mqttform;
    	let current;
    	let mounted;
    	let dispose;
    	wifiform = new WiFiForm({ $$inline: true });
    	mqttform = new MqttForm({ $$inline: true });

    	const block = {
    		c: function create() {
    			section = element("section");
    			div = element("div");
    			button = element("button");
    			button.textContent = "Restart Server";
    			t1 = space();
    			form = element("form");
    			create_component(wifiform.$$.fragment);
    			t2 = space();
    			create_component(mqttform.$$.fragment);
    			attr_dev(button, "type", "button");
    			add_location(button, file, 27, 6, 587);
    			add_location(form, file, 28, 6, 660);
    			attr_dev(div, "class", "container svelte-vk6hfs");
    			add_location(div, file, 25, 4, 556);
    			attr_dev(section, "class", "svelte-vk6hfs");
    			add_location(section, file, 24, 2, 542);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, section, anchor);
    			append_dev(section, div);
    			append_dev(div, button);
    			append_dev(div, t1);
    			append_dev(div, form);
    			mount_component(wifiform, form, null);
    			append_dev(form, t2);
    			mount_component(mqttform, form, null);
    			current = true;

    			if (!mounted) {
    				dispose = listen_dev(button, "click", /*onRestart*/ ctx[1], false, false, false);
    				mounted = true;
    			}
    		},
    		p: noop,
    		i: function intro(local) {
    			if (current) return;
    			transition_in(wifiform.$$.fragment, local);
    			transition_in(mqttform.$$.fragment, local);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(wifiform.$$.fragment, local);
    			transition_out(mqttform.$$.fragment, local);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(section);
    			destroy_component(wifiform);
    			destroy_component(mqttform);
    			mounted = false;
    			dispose();
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_else_block.name,
    		type: "else",
    		source: "(24:2) {:else}",
    		ctx
    	});

    	return block;
    }

    // (15:2) {#if $restarted}
    function create_if_block(ctx) {
    	let section;
    	let div;
    	let h1;
    	let t1;
    	let p;

    	const block = {
    		c: function create() {
    			section = element("section");
    			div = element("div");
    			h1 = element("h1");
    			h1.textContent = "Server Restarted";
    			t1 = space();
    			p = element("p");
    			p.textContent = "The server has been restarted. If the wifi settings have changed, then the IP address of your server may have changed as well.";
    			add_location(h1, file, 17, 6, 324);
    			add_location(p, file, 18, 6, 356);
    			attr_dev(div, "class", "container svelte-vk6hfs");
    			add_location(div, file, 16, 4, 294);
    			attr_dev(section, "class", "svelte-vk6hfs");
    			add_location(section, file, 15, 2, 280);
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, section, anchor);
    			append_dev(section, div);
    			append_dev(div, h1);
    			append_dev(div, t1);
    			append_dev(div, p);
    		},
    		p: noop,
    		i: noop,
    		o: noop,
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(section);
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_if_block.name,
    		type: "if",
    		source: "(15:2) {#if $restarted}",
    		ctx
    	});

    	return block;
    }

    function create_fragment(ctx) {
    	let main;
    	let current_block_type_index;
    	let if_block;
    	let current;
    	const if_block_creators = [create_if_block, create_else_block];
    	const if_blocks = [];

    	function select_block_type(ctx, dirty) {
    		if (/*$restarted*/ ctx[0]) return 0;
    		return 1;
    	}

    	current_block_type_index = select_block_type(ctx);
    	if_block = if_blocks[current_block_type_index] = if_block_creators[current_block_type_index](ctx);

    	const block = {
    		c: function create() {
    			main = element("main");
    			if_block.c();
    			add_location(main, file, 13, 0, 252);
    		},
    		l: function claim(nodes) {
    			throw new Error("options.hydrate only works if the component was compiled with the `hydratable: true` option");
    		},
    		m: function mount(target, anchor) {
    			insert_dev(target, main, anchor);
    			if_blocks[current_block_type_index].m(main, null);
    			current = true;
    		},
    		p: function update(ctx, [dirty]) {
    			let previous_block_index = current_block_type_index;
    			current_block_type_index = select_block_type(ctx);

    			if (current_block_type_index === previous_block_index) {
    				if_blocks[current_block_type_index].p(ctx, dirty);
    			} else {
    				group_outros();

    				transition_out(if_blocks[previous_block_index], 1, 1, () => {
    					if_blocks[previous_block_index] = null;
    				});

    				check_outros();
    				if_block = if_blocks[current_block_type_index];

    				if (!if_block) {
    					if_block = if_blocks[current_block_type_index] = if_block_creators[current_block_type_index](ctx);
    					if_block.c();
    				} else {
    					if_block.p(ctx, dirty);
    				}

    				transition_in(if_block, 1);
    				if_block.m(main, null);
    			}
    		},
    		i: function intro(local) {
    			if (current) return;
    			transition_in(if_block);
    			current = true;
    		},
    		o: function outro(local) {
    			transition_out(if_block);
    			current = false;
    		},
    		d: function destroy(detaching) {
    			if (detaching) detach_dev(main);
    			if_blocks[current_block_type_index].d();
    		}
    	};

    	dispatch_dev("SvelteRegisterBlock", {
    		block,
    		id: create_fragment.name,
    		type: "component",
    		source: "",
    		ctx
    	});

    	return block;
    }

    function instance($$self, $$props, $$invalidate) {
    	let $restarted;
    	validate_store(restarted, 'restarted');
    	component_subscribe($$self, restarted, $$value => $$invalidate(0, $restarted = $$value));
    	let { $$slots: slots = {}, $$scope } = $$props;
    	validate_slots('App', slots, []);

    	async function onRestart() {
    		await restart();
    		restarted.set(true);
    	}

    	const writable_props = [];

    	Object.keys($$props).forEach(key => {
    		if (!~writable_props.indexOf(key) && key.slice(0, 2) !== '$$' && key !== 'slot') console.warn(`<App> was created with unknown prop '${key}'`);
    	});

    	$$self.$capture_state = () => ({
    		MqttForm,
    		WiFiForm,
    		restarted,
    		restart,
    		onRestart,
    		$restarted
    	});

    	return [$restarted, onRestart];
    }

    class App extends SvelteComponentDev {
    	constructor(options) {
    		super(options);
    		init(this, options, instance, create_fragment, safe_not_equal, {});

    		dispatch_dev("SvelteRegisterComponent", {
    			component: this,
    			tagName: "App",
    			options,
    			id: create_fragment.name
    		});
    	}
    }

    const app = new App({
    	target: document.body,
    });

    return app;

})();
//# sourceMappingURL=bundle.js.map
