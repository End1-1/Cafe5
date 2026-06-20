import {
  ingestMenu,
  isPackage,
  MenuGoods,
  DIETARY_BADGES,
  ATTRIBUTE_KEYS,
  uniqueAttributeOptions,
  findPackageComponent,
  fixedAttributeOptionPrice,
  packageNeedsAttributePicker,
  packageLinePrice,
  resolvePackageCartLine,
  attributeDisplayLabel,
} from "./menu.js";

// --------------------------------------------------------------------------
// i18n
// --------------------------------------------------------------------------

const I18N = {
  en: {
    banner: "Banner place",
    takeAway: "Take away",
    dineIn: "Dine in",
    selectLanguage: "Select language",
    scanApp: "SCAN AND DOWNLOAD APP",
    langName: "English",
    home: "Home",
    search: "Search",
    cancelOrder: "Cancel order",
    goToCart: "Go to cart",
    cart: "Cart: {n}   {sum} ֏",
    loading: "Loading menu...",
    loadFailed: "Menu load failed",
    retry: "Retry",
    notConfigured: "Backend is not configured.\nEdit config.json (host, username, password) and restart the server.",
    type: "Type",
    size: "Size",
    calories: "Calories",
    fats: "Fats",
    carbs: "Carbs",
    protein: "Protein",
    add: "Add",
    addPrice: "Add — {sum} ֏",
    unavailable: "This combination is not available. Please change your selection.",
    instructions: "Add any special instructions (allergies, important and details)",
    detailsType: "Type: {v}",
    detailsSize: "Size: {v}",
    detailsBju: "Per 100 g: {kcal} kcal · P {p} g · F {f} g · C {c} g",
    noDetails: "Detailed description will be added later.",
    yourOrder: "Your order",
    total: "Total",
    emptyCart: "Your cart is empty",
    noDishes: "No dishes in this group",
    noOptions: "This package has no configured options.",
  },
  ru: {
    banner: "Место для баннера",
    takeAway: "С собой",
    dineIn: "В зале",
    selectLanguage: "Выберите язык",
    scanApp: "СКАНИРУЙТЕ И СКАЧАЙТЕ ПРИЛОЖЕНИЕ",
    langName: "Русский",
    home: "Главная",
    search: "Поиск",
    cancelOrder: "Отменить заказ",
    goToCart: "В корзину",
    cart: "Корзина: {n}   {sum} ֏",
    loading: "Загрузка меню...",
    loadFailed: "Не удалось загрузить меню",
    retry: "Повторить",
    notConfigured: "Сервер не настроен.\nЗаполните config.json (host, username, password) и перезапустите сервер.",
    type: "Тип",
    size: "Размер",
    calories: "Калории",
    fats: "Жиры",
    carbs: "Углеводы",
    protein: "Белки",
    add: "Добавить",
    addPrice: "Добавить — {sum} ֏",
    unavailable: "Такая комбинация недоступна. Измените выбор.",
    instructions: "Особые пожелания (аллергии, важные детали)",
    detailsType: "Тип: {v}",
    detailsSize: "Размер: {v}",
    detailsBju: "На 100 г: {kcal} ккал · Б {p} г · Ж {f} г · У {c} г",
    noDetails: "Подробное описание будет добавлено позже.",
    yourOrder: "Ваш заказ",
    total: "Итого",
    emptyCart: "Корзина пуста",
    noDishes: "В этой группе нет блюд",
    noOptions: "У этого набора нет настроенных опций.",
  },
  hy: {
    banner: "Բանների տեղ",
    takeAway: "Տանել",
    dineIn: "Տեղում",
    selectLanguage: "Ընտրեք լեզուն",
    scanApp: "ՍԿԱՆԵՔ ԵՎ ՆԵՐԲԵՌՆԵՔ ՀԱՎԵԼՎԱԾԸ",
    langName: "Հայերեն",
    home: "Գլխավոր",
    search: "Որոնում",
    cancelOrder: "Չեղարկել պատվերը",
    goToCart: "Զամբյուղ",
    cart: "Զամբյուղ: {n}   {sum} ֏",
    loading: "Մենյուի բեռնում...",
    loadFailed: "Մենյուն չհաջողվեց բեռնել",
    retry: "Կրկնել",
    notConfigured: "Սերվերը կարգավորված չէ։\nԼրացրեք config.json (host, username, password) և վերագործարկեք սերվերը։",
    type: "Տեսակ",
    size: "Չափ",
    calories: "Կալորիա",
    fats: "Ճարպեր",
    carbs: "Ածխաջրեր",
    protein: "Սպիտակուց",
    add: "Ավելացնել",
    addPrice: "Ավելացնել — {sum} ֏",
    unavailable: "Այս համակցությունը հասանելի չէ։ Փոխեք ընտրությունը։",
    instructions: "Հատուկ ցանկություններ (ալերգիա, կարևոր մանրամասներ)",
    detailsType: "Տեսակ՝ {v}",
    detailsSize: "Չափ՝ {v}",
    detailsBju: "100 գ-ի համար՝ {kcal} կկալ · Ս {p} գ · Ճ {f} գ · Ա {c} գ",
    noDetails: "Մանրամասն նկարագրությունը կավելացվի ավելի ուշ։",
    yourOrder: "Ձեր պատվերը",
    total: "Ընդամենը",
    emptyCart: "Զամբյուղը դատարկ է",
    noDishes: "Այս խմբում ուտեստներ չկան",
    noOptions: "Այս փաթեթը չունի կարգավորված տարբերակներ։",
  },
};

function t(key, vars) {
  let s = (I18N[state.locale] && I18N[state.locale][key]) || I18N.en[key] || key;
  if (vars) {
    for (const k of Object.keys(vars)) {
      s = s.replace(`{${k}}`, vars[k]);
    }
  }
  return s;
}

function fmtPrice(value) {
  return Math.round(value || 0).toLocaleString("en-US");
}

// --------------------------------------------------------------------------
// State
// --------------------------------------------------------------------------

const state = {
  locale: "en",
  locales: ["ru", "en", "hy"],
  serviceMode: "takeaway", // or "dinein"
  groups: [],
  dishes: [],
  currentGroupId: 0,
  search: "",
  menuLoaded: false,
  loadingLocale: null,
  cart: new Map(), // id -> { dish, qty }
};

const $ = (sel) => document.querySelector(sel);
const $$ = (sel) => Array.from(document.querySelectorAll(sel));

const PLACEHOLDER_IMG = "res/dish_placeholder.png";

function imgSrc(path) {
  return path && path.length ? path : PLACEHOLDER_IMG;
}

// --------------------------------------------------------------------------
// Data
// --------------------------------------------------------------------------

async function loadConfig() {
  try {
    const res = await fetch("api/config.php");
    const cfg = await res.json();
    if (Array.isArray(cfg.locales) && cfg.locales.length) state.locales = cfg.locales;
    if (cfg.defaultLocale) state.locale = cfg.defaultLocale;
    state.configured = cfg.configured;
  } catch (e) {
    // Keep defaults.
  }
}

async function loadMenu(locale) {
  const res = await fetch(`api/menu.php?locale=${encodeURIComponent(locale)}`);
  const text = await res.text();
  let json;
  try {
    json = JSON.parse(text);
  } catch (e) {
    throw new Error(
      `Backend did not return JSON (HTTP ${res.status}). ` +
        `Make sure api/menu.php is reachable and config.json is set.`
    );
  }
  if (json.status !== 1) {
    throw new Error(json.message || "Menu request failed");
  }
  const data = ingestMenu(json);
  state.groups = data.groups;
  state.dishes = data.dishes;
  state.menuLoaded = true;
  state.loadingLocale = locale;
  if (state.groups.length) {
    state.currentGroupId = state.groups[0].id;
  }
}

// --------------------------------------------------------------------------
// Start screen
// --------------------------------------------------------------------------

function applyStaticI18n() {
  $$("[data-i18n]").forEach((el) => {
    el.textContent = t(el.getAttribute("data-i18n"));
  });
  $$("[data-i18n-ph]").forEach((el) => {
    el.setAttribute("placeholder", t(el.getAttribute("data-i18n-ph")));
  });
  document.documentElement.lang = state.locale;
}

function refreshLangButtons() {
  $$(".lang-btn").forEach((btn) => {
    btn.classList.toggle("checked", btn.dataset.locale === state.locale);
  });
}

function setLocale(locale) {
  if (!I18N[locale]) return;
  state.locale = locale;
  refreshLangButtons();
  applyStaticI18n();
}

function showStartOverlay({ text, spinning = true, retry = false }) {
  $("#start-overlay").classList.remove("hidden");
  $("#start-overlay-text").textContent = text;
  $("#start-spinner").classList.toggle("hidden", !spinning);
  $("#start-retry").classList.toggle("hidden", !retry);
}

function hideStartOverlay() {
  $("#start-overlay").classList.add("hidden");
}

async function ensureMenuLoaded() {
  if (state.menuLoaded && state.loadingLocale === state.locale) return true;
  showStartOverlay({ text: t("loading"), spinning: true });
  try {
    await loadMenu(state.locale);
    hideStartOverlay();
    return true;
  } catch (e) {
    showStartOverlay({
      text: `${t("loadFailed")}\n${e.message}`,
      spinning: false,
      retry: true,
    });
    return false;
  }
}

async function enterMenu(mode) {
  state.serviceMode = mode;
  const ok = await ensureMenuLoaded();
  if (!ok) return;
  showScreen("menu");
  renderMenuScreen();
}

function showScreen(name) {
  $("#screen-start").classList.toggle("hidden", name !== "start");
  $("#screen-menu").classList.toggle("hidden", name !== "menu");
}

// --------------------------------------------------------------------------
// Menu screen rendering
// --------------------------------------------------------------------------

function dishesByGroup(groupId) {
  return state.dishes.filter((d) => d.groupId === groupId);
}

function filteredDishes() {
  const all = dishesByGroup(state.currentGroupId);
  const needle = state.search.trim().toLowerCase();
  if (!needle) return all;
  return all.filter(
    (d) =>
      d.name.toLowerCase().includes(needle) ||
      (d.groupName && d.groupName.toLowerCase().includes(needle))
  );
}

function dishBadges(dish) {
  return DIETARY_BADGES.filter((b) => dish[b.key]);
}

function renderMenuScreen() {
  $("#service-mode-label").textContent = t(state.serviceMode === "takeaway" ? "takeAway" : "dineIn");
  $("#service-mode-icon").src =
    state.serviceMode === "takeaway" ? "res/icon_takeaway.png" : "res/icon_dinein.png";
  $("#btn-language").textContent = t("langName");

  renderGroups();
  renderChips();
  renderDishGrid();
  updateCartSummary();
}

function renderGroups() {
  const host = $("#group-list");
  host.innerHTML = "";
  state.groups.forEach((g) => {
    const btn = document.createElement("button");
    btn.className = "btn-group" + (g.id === state.currentGroupId ? " checked" : "");
    btn.innerHTML = `<img src="${imgSrc(g.iconPath)}" alt="" onerror="this.src='${PLACEHOLDER_IMG}'"/><span></span>`;
    btn.querySelector("span").textContent = g.name;
    btn.addEventListener("click", () => selectGroup(g.id));
    host.appendChild(btn);
  });
}

function renderChips() {
  const host = $("#chips");
  host.innerHTML = "";
  state.groups.forEach((g) => {
    const btn = document.createElement("button");
    btn.className = "chip" + (g.id === state.currentGroupId ? " checked" : "");
    btn.textContent = g.name;
    btn.addEventListener("click", () => selectGroup(g.id));
    host.appendChild(btn);
  });
}

function selectGroup(groupId) {
  state.currentGroupId = groupId;
  renderGroups();
  renderChips();
  renderDishGrid();
  const activeChip = $("#chips .chip.checked");
  if (activeChip) activeChip.scrollIntoView({ inline: "center", block: "nearest", behavior: "smooth" });
}

function renderDishGrid() {
  const grid = $("#dish-grid");
  grid.innerHTML = "";
  const dishes = filteredDishes();

  if (!dishes.length) {
    const note = document.createElement("div");
    note.className = "empty-note";
    note.textContent = t("noDishes");
    grid.appendChild(note);
    return;
  }

  dishes.forEach((dish) => grid.appendChild(buildDishCard(dish)));
}

function dishSubtitle(dish) {
  if (dish.attrType || dish.attrSize) {
    const parts = [];
    if (dish.attrType) parts.push(dish.attrType);
    if (dish.attrSize) {
      parts.push(dish.attrMeasurement ? `${dish.attrSize} ${dish.attrMeasurement}` : dish.attrSize);
    }
    return parts.join(" · ");
  }
  return dish.groupName || "";
}

function buildDishCard(dish) {
  const card = document.createElement("div");
  card.className = "dish-card";

  const badges = dishBadges(dish)
    .map((b) => `<span class="badge" title="${b.title}">${b.short}</span>`)
    .join("");

  card.innerHTML = `
    <img class="dish-img" src="${imgSrc(dish.imagePath)}" alt="" onerror="this.src='${PLACEHOLDER_IMG}'"/>
    <div class="dish-title"></div>
    <div class="dish-sub"></div>
    ${badges ? `<div class="badges">${badges}</div>` : ""}
    <div class="dish-bottom">
      <span class="dish-time">${dish.prepTime || ""}</span>
      <span class="dish-price">${fmtPrice(dish.price)} ֏</span>
      <button class="dish-info" title="info">i</button>
    </div>`;
  card.querySelector(".dish-title").textContent = dish.name;
  card.querySelector(".dish-sub").textContent = dishSubtitle(dish);

  const infoBtn = card.querySelector(".dish-info");
  infoBtn.addEventListener("click", (e) => {
    e.stopPropagation();
    openDishDetails(dish);
  });
  card.addEventListener("click", () => onAddToCart(dish));
  return card;
}

// --------------------------------------------------------------------------
// Cart
// --------------------------------------------------------------------------

function cartItemCount() {
  let n = 0;
  for (const { qty } of state.cart.values()) n += qty;
  return n;
}

function cartTotal() {
  let sum = 0;
  for (const { dish, qty } of state.cart.values()) sum += dish.price * qty;
  return sum;
}

function addDishToCart(dish, qty = 1) {
  if (!dish || dish.id <= 0 || qty <= 0) return;
  const existing = state.cart.get(dish.id);
  if (existing) {
    existing.qty += qty;
  } else {
    state.cart.set(dish.id, { dish, qty });
  }
  updateCartSummary();
}

function setCartQty(id, qty) {
  const entry = state.cart.get(id);
  if (!entry) return;
  if (qty <= 0) {
    state.cart.delete(id);
  } else {
    entry.qty = qty;
  }
  updateCartSummary();
}

function updateCartSummary() {
  $("#cart-summary").textContent = t("cart", { n: cartItemCount(), sum: fmtPrice(cartTotal()) });
  $("#btn-cart").disabled = state.cart.size === 0;
}

function onAddToCart(dish) {
  if (isPackage(dish)) {
    if (packageNeedsAttributePicker(dish.packageComponents)) {
      openPackagePicker(dish);
    } else {
      const component = dish.packageComponents[0] || null;
      addDishToCart(resolvePackageCartLine(dish, component));
    }
    return;
  }
  if (dish.type === MenuGoods.TYPE_GOODS || dish.type === MenuGoods.TYPE_BEER) {
    addDishToCart(dish);
  }
}

// --------------------------------------------------------------------------
// Dish details modal
// --------------------------------------------------------------------------

function openDishDetails(dish) {
  $("#details-title").textContent = dish.name;
  const lines = [];
  if (dish.attrType) lines.push(t("detailsType", { v: dish.attrType }));
  if (dish.attrSize) {
    const sizeText = dish.attrMeasurement ? `${dish.attrSize} ${dish.attrMeasurement}` : dish.attrSize;
    lines.push(t("detailsSize", { v: sizeText }));
  }
  const hasBju = dish.kcal > 0 || dish.protein > 0 || dish.fat > 0 || dish.carbs > 0;
  if (hasBju) {
    lines.push(
      t("detailsBju", {
        kcal: dish.kcal >= 100 ? Math.round(dish.kcal) : dish.kcal.toFixed(1),
        p: dish.protein.toFixed(1),
        f: dish.fat.toFixed(1),
        c: dish.carbs.toFixed(1),
      })
    );
  }
  if (dish.description) lines.unshift(dish.description);
  $("#details-hint").textContent = lines.length ? lines.join("\n") : t("noDetails");
  openModal("details");
}

// --------------------------------------------------------------------------
// Package picker modal
// --------------------------------------------------------------------------

const pkgState = { pkg: null, qty: 1, selections: {} };

function bjuText(value, unit, decimals) {
  if (!(value > 0)) return "—";
  const v = decimals === 0 ? Math.round(value) : value.toFixed(decimals);
  return `${v} ${unit}`;
}

function openPackagePicker(pkg) {
  pkgState.pkg = pkg;
  pkgState.qty = 1;
  pkgState.selections = {};

  $("#pkg-image").src = imgSrc(pkg.imagePath);
  $("#pkg-image").onerror = function () { this.src = PLACEHOLDER_IMG; };
  $("#pkg-title").textContent = pkg.name;
  $("#pkg-desc").textContent = pkg.description || "";
  $("#pkg-qty").textContent = "1";

  $("#pkg-badges").innerHTML = dishBadges(pkg)
    .map((b) => `<span class="badge" title="${b.title}">${b.short}</span>`)
    .join("");

  $("#pkg-kcal").textContent = pkg.kcal > 0 ? bjuText(pkg.kcal, "kcal", pkg.kcal >= 100 ? 0 : 1) : "—";
  $("#pkg-fat").textContent = bjuText(pkg.fat, "g", 1);
  $("#pkg-carbs").textContent = bjuText(pkg.carbs, "g", 1);
  $("#pkg-protein").textContent = bjuText(pkg.protein, "g", 1);

  buildAttributeGroups(pkg);
  openModal("pkg");
}

function sectionTitle(key) {
  if (key === "Type") return t("type");
  if (key === "Size") return t("size");
  return key;
}

function measurementForKey(pkg, key, value) {
  for (const c of pkg.packageComponents) {
    const actual = key === "Type" ? c.attributes.type : key === "Size" ? c.attributes.size : "";
    if (actual === value) return key === "Size" ? c.attributes.measurement : "";
  }
  return "";
}

function optionLabel(pkg, key, value) {
  const measurement = measurementForKey(pkg, key, value);
  const name = attributeDisplayLabel(value, measurement);
  const price = fixedAttributeOptionPrice(pkg.packageComponents, key, value);
  return `${name}  +${fmtPrice(price)}`;
}

function buildAttributeGroups(pkg) {
  const host = $("#pkg-attributes");
  host.innerHTML = "";
  pkgState.selections = {};

  if (!pkg.packageComponents.length) {
    host.innerHTML = `<div class="pkg-unavailable">${t("noOptions")}</div>`;
    $("#pkg-add").disabled = true;
    return;
  }

  const options = uniqueAttributeOptions(pkg.packageComponents, ATTRIBUTE_KEYS);
  const first = pkg.packageComponents[0];
  const initial = { Type: first.attributes.type, Size: first.attributes.size };

  ATTRIBUTE_KEYS.forEach((key) => {
    const values = options[key] || [];
    if (!values.length) return;

    const section = document.createElement("div");
    section.className = "attr-section";
    const header = document.createElement("div");
    header.className = "attr-header";
    header.textContent = sectionTitle(key);
    section.appendChild(header);

    values.forEach((value) => {
      const row = document.createElement("label");
      row.className = "attr-row";
      const radio = document.createElement("input");
      radio.type = "radio";
      radio.name = `attr-${key}`;
      radio.value = value;
      if (value === initial[key]) {
        radio.checked = true;
        pkgState.selections[key] = value;
      }
      radio.addEventListener("change", () => {
        if (radio.checked) {
          pkgState.selections[key] = value;
          updatePackagePreview();
        }
      });
      const label = document.createElement("span");
      label.className = "attr-label";
      label.dataset.key = key;
      label.dataset.value = value;
      label.textContent = optionLabel(pkg, key, value);
      row.appendChild(radio);
      row.appendChild(label);
      section.appendChild(row);
    });

    host.appendChild(section);
  });

  updatePackagePreview();
}

function updatePackagePreview() {
  const pkg = pkgState.pkg;
  const component = findPackageComponent(pkg.packageComponents, pkgState.selections);
  const valid = component != null;

  const addBtn = $("#pkg-add");
  addBtn.disabled = !valid;
  if (valid) {
    const total = packageLinePrice(pkg, component) * pkgState.qty;
    addBtn.textContent = t("addPrice", { sum: fmtPrice(total) });
  } else {
    addBtn.textContent = t("add");
  }
  $("#pkg-unavailable").classList.toggle("hidden", valid);
}

function changePkgQty(delta) {
  pkgState.qty = Math.max(1, pkgState.qty + delta);
  $("#pkg-qty").textContent = String(pkgState.qty);
  updatePackagePreview();
}

function confirmPackage() {
  const pkg = pkgState.pkg;
  const component = findPackageComponent(pkg.packageComponents, pkgState.selections);
  if (!component) return;
  addDishToCart(resolvePackageCartLine(pkg, component), pkgState.qty);
  closeModal("pkg");
}

// --------------------------------------------------------------------------
// Cart modal
// --------------------------------------------------------------------------

function openCart() {
  renderCart();
  openModal("cart");
}

function lineSubtitle(dish) {
  const parts = [];
  if (dish.attrType) parts.push(dish.attrType);
  if (dish.attrSize) {
    parts.push(dish.attrMeasurement ? `${dish.attrSize} ${dish.attrMeasurement}` : dish.attrSize);
  }
  if (!parts.length && dish.groupName) parts.push(dish.groupName);
  return parts.join(" · ");
}

function renderCart() {
  const host = $("#cart-lines");
  host.innerHTML = "";

  if (state.cart.size === 0) {
    host.innerHTML = `<div class="cart-empty">${t("emptyCart")}</div>`;
    $("#cart-total-value").textContent = `0 ֏`;
    return;
  }

  for (const [id, { dish, qty }] of state.cart) {
    const line = document.createElement("div");
    line.className = "cart-line";
    line.innerHTML = `
      <img src="${imgSrc(dish.imagePath)}" alt="" onerror="this.src='${PLACEHOLDER_IMG}'"/>
      <div class="cl-info">
        <div class="cl-name"></div>
        <div class="cl-sub"></div>
      </div>
      <div class="qty-wrap">
        <button class="qty-btn qty-minus">&minus;</button>
        <div class="qty-value">${qty}</div>
        <button class="qty-btn qty-plus">+</button>
      </div>
      <div class="cl-price">${fmtPrice(dish.price * qty)} ֏</div>`;
    line.querySelector(".cl-name").textContent = dish.name;
    line.querySelector(".cl-sub").textContent = lineSubtitle(dish);
    line.querySelector(".qty-minus").addEventListener("click", () => {
      setCartQty(id, qty - 1);
      renderCart();
    });
    line.querySelector(".qty-plus").addEventListener("click", () => {
      setCartQty(id, qty + 1);
      renderCart();
    });
    host.appendChild(line);
  }

  $("#cart-total-value").textContent = `${fmtPrice(cartTotal())} ֏`;
}

// --------------------------------------------------------------------------
// Modals
// --------------------------------------------------------------------------

function openModal(name) {
  $(`#${name}-modal`).classList.remove("hidden");
}

function closeModal(name) {
  $(`#${name}-modal`).classList.add("hidden");
}

// --------------------------------------------------------------------------
// Wiring
// --------------------------------------------------------------------------

function cycleLocale() {
  const idx = state.locales.indexOf(state.locale);
  const next = state.locales[(idx + 1) % state.locales.length];
  setLocale(next);
  state.menuLoaded = false; // names are localized server-side
  ensureMenuLoadedAndRerender();
}

async function ensureMenuLoadedAndRerender() {
  showScreen("menu");
  const dishScroll = $(".dish-scroll");
  dishScroll.style.opacity = "0.4";
  try {
    await loadMenu(state.locale);
    renderMenuScreen();
  } catch (e) {
    // fall back to start with error
    showScreen("start");
    showStartOverlay({ text: `${t("loadFailed")}\n${e.message}`, spinning: false, retry: true });
  } finally {
    dishScroll.style.opacity = "1";
  }
}

function wire() {
  // Language buttons (start screen)
  $$(".lang-btn").forEach((btn) => {
    btn.addEventListener("click", () => setLocale(btn.dataset.locale));
  });

  $("#btn-takeaway").addEventListener("click", () => enterMenu("takeaway"));
  $("#btn-dinein").addEventListener("click", () => enterMenu("dinein"));
  $("#start-retry").addEventListener("click", () => ensureMenuLoaded());

  // Menu screen
  $("#btn-home").addEventListener("click", () => showScreen("start"));
  $("#btn-language").addEventListener("click", cycleLocale);
  $("#btn-service-mode").addEventListener("click", () => {
    state.serviceMode = state.serviceMode === "takeaway" ? "dinein" : "takeaway";
    renderMenuScreen();
  });
  $("#le-search").addEventListener("input", (e) => {
    state.search = e.target.value;
    renderDishGrid();
  });
  $("#btn-cancel-order").addEventListener("click", () => {
    state.cart.clear();
    updateCartSummary();
    showScreen("start");
  });
  $("#btn-cart").addEventListener("click", openCart);

  // Package picker
  $("#pkg-qty-minus").addEventListener("click", () => changePkgQty(-1));
  $("#pkg-qty-plus").addEventListener("click", () => changePkgQty(1));
  $("#pkg-add").addEventListener("click", confirmPackage);

  // Modal closing (button + backdrop click)
  $$(".modal-close").forEach((btn) => {
    btn.addEventListener("click", () => closeModal(btn.dataset.close));
  });
  $$(".modal-backdrop").forEach((backdrop) => {
    backdrop.addEventListener("click", (e) => {
      if (e.target === backdrop) backdrop.classList.add("hidden");
    });
  });
  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") $$(".modal-backdrop").forEach((m) => m.classList.add("hidden"));
  });
}

async function init() {
  await loadConfig();
  setLocale(state.locale);
  applyStaticI18n();
  refreshLangButtons();
  wire();
}

init();
