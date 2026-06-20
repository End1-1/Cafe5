// Menu data model + helpers, ported from the Qt kiosk (menuhelpers.cpp,
// menutypes.h, menucache.cpp). Pure functions, no DOM access.

export const MenuGoods = {
  TYPE_GOODS: 1,
  TYPE_BEER: 2,
  TYPE_PACKAGE: 5,
};

export const ATTRIBUTE_KEYS = ["Type", "Size"];

// Dietary / allergen badges (kiosk used :/dietary/*.png icons).
export const DIETARY_BADGES = [
  { key: "glutenFree", short: "GF", title: "Gluten free" },
  { key: "vegetarian", short: "VEG", title: "Vegetarian" },
  { key: "vegan", short: "VGN", title: "Vegan" },
  { key: "noGmo", short: "GMO\u2715", title: "No GMO" },
  { key: "noLactose", short: "LF", title: "Lactose free" },
  { key: "noSugar", short: "SF", title: "Sugar free" },
  { key: "containsNuts", short: "NUTS", title: "Contains nuts" },
  { key: "halalKosher", short: "H/K", title: "Halal / Kosher" },
];

function trimStr(v) {
  return typeof v === "string" ? v.trim() : v == null ? "" : String(v).trim();
}

function toNumber(v, fallback = 0) {
  const n = typeof v === "number" ? v : parseFloat(v);
  return Number.isFinite(n) ? n : fallback;
}

function toBool(v) {
  return v === true || v === 1 || v === "1" || v === "true";
}

// dynStr: object -> .value, else the string itself (trimmed).
function dynStr(v) {
  if (v && typeof v === "object") {
    return trimStr(v.value);
  }
  return trimStr(v);
}

// dynPrice: object -> .price, else 0.
function dynPrice(v) {
  if (v && typeof v === "object") {
    return toNumber(v.price, 0);
  }
  return 0;
}

function dynMeasurement(dynObj, key) {
  const val = dynObj ? dynObj[key] : undefined;
  if (val && typeof val === "object") {
    const m = trimStr(val.measurement);
    if (m) return m;
  }
  if (key === "Size" && dynObj) {
    return trimStr(dynObj.Measurement);
  }
  return "";
}

// c_goods.f_data is sometimes an object, sometimes a JSON string.
function parseGoodsData(vData) {
  if (vData && typeof vData === "object") return vData;
  const raw = trimStr(vData);
  if (!raw) return {};
  try {
    const parsed = JSON.parse(raw);
    return parsed && typeof parsed === "object" ? parsed : {};
  } catch (e) {
    return {};
  }
}

function dietaryFromData(dataObj) {
  const badge = (dataObj && dataObj.f_dietary_badge) || {};
  return {
    glutenFree: toBool(badge.gluten_free),
    vegetarian: toBool(badge.vegetarian),
    vegan: toBool(badge.vegan),
    noGmo: toBool(badge.no_gmo),
    noLactose: toBool(badge.no_lactose),
    noSugar: toBool(badge.no_sugar),
    containsNuts: toBool(badge.contains_nuts),
    halalKosher: toBool(badge.halal_kosher),
  };
}

function bjuFromData(dataObj) {
  const bju = (dataObj && dataObj.f_bju) || {};
  return {
    kcal: toNumber(bju.kcal),
    protein: toNumber(bju.protein),
    fat: toNumber(bju.fat),
    carbs: toNumber(bju.carbs),
  };
}

function dynamicAttrsFromData(dataObj) {
  const dyn = (dataObj && dataObj.f_dynamic_attributes) || {};
  let measurement = trimStr(dyn.Measurement);
  if (!measurement) measurement = dynMeasurement(dyn, "Size");
  return {
    type: dynStr(dyn.Type),
    size: dynStr(dyn.Size),
    measurement,
    typePrice: dynPrice(dyn.Type),
    sizePrice: dynPrice(dyn.Size),
  };
}

// --- Image payload (base64) -> data URI ---------------------------------

function imageDataUri(payload) {
  if (!payload) return "";
  let text = String(payload).trim();
  if (!text) return "";
  if (/^data:image/i.test(text)) {
    return text;
  }
  text = text.replace(/\s+/g, "");
  let mime = "image/jpeg";
  if (text.startsWith("iVBORw0KGgo")) mime = "image/png";
  else if (text.startsWith("/9j/")) mime = "image/jpeg";
  else if (text.startsWith("R0lGOD")) mime = "image/gif";
  else if (text.startsWith("UklGR")) mime = "image/webp";
  else if (text.startsWith("Qk")) mime = "image/bmp";
  return `data:${mime};base64,${text}`;
}

// --- Parsing -------------------------------------------------------------

export function parsePackageComponent(row) {
  const dataObj = parseGoodsData(row.f_data);
  return Object.assign(
    {
      goodsId: toNumber(row.f_goods),
      name: trimStr(row.f_goods_name),
      price: toNumber(row.f_price),
      qty: toNumber(row.f_qty, 1),
      imagePath: imageDataUri(row.f_image),
      attributes: dynamicAttrsFromData(dataObj),
    },
    dietaryFromData(dataObj),
    bjuFromData(dataObj)
  );
}

export function fillDishFromMenuRow(row) {
  const dataObj = parseGoodsData(row.f_data);
  const dyn = dynamicAttrsFromData(dataObj);
  const dish = Object.assign(
    {
      id: toNumber(row.f_dish),
      groupId: toNumber(row.f_group),
      type: toNumber(row.f_type, MenuGoods.TYPE_GOODS),
      name: trimStr(row.f_goods_name),
      groupName: trimStr(row.f_group_name),
      description: trimStr(row.f_description),
      price: toNumber(row.f_price),
      popular: toNumber(row.f_recent) > 0,
      prepTime: "10 - 15 Min",
      imagePath: imageDataUri(row.f_image),
      attrType: dyn.type,
      attrSize: dyn.size,
      attrMeasurement: dyn.measurement,
      packageId: 0,
      packageName: "",
      packageComponents: [],
    },
    dietaryFromData(dataObj),
    bjuFromData(dataObj)
  );

  if (dish.type === MenuGoods.TYPE_PACKAGE && Array.isArray(row.f_complectation)) {
    dish.packageComponents = row.f_complectation.map(parsePackageComponent);
  }
  return dish;
}

export function ingestMenu(json) {
  const groups = (json.groups || []).map((g) => ({
    id: toNumber(g.f_id),
    name: trimStr(g.f_name),
    iconPath: imageDataUri(g.f_image),
  }));
  const dishes = (json.dishes || []).map(fillDishFromMenuRow);
  return { groups, dishes };
}

// --- Package attribute helpers ------------------------------------------

export function isPackage(dish) {
  return dish && dish.type === MenuGoods.TYPE_PACKAGE;
}

function componentAttr(component, key) {
  if (key === "Type") return component.attributes.type;
  if (key === "Size") return component.attributes.size;
  return "";
}

function componentMatches(component, selections) {
  for (const key of Object.keys(selections)) {
    const selected = selections[key];
    if (!selected) continue;
    if (componentAttr(component, key) !== selected) return false;
  }
  return true;
}

export function uniqueAttributeOptions(components, keys = ATTRIBUTE_KEYS) {
  const result = {};
  for (const key of keys) {
    const values = [];
    for (const component of components) {
      const value = componentAttr(component, key);
      if (!value || values.includes(value)) continue;
      values.push(value);
    }
    result[key] = values;
  }
  return result;
}

export function findPackageComponent(components, selections) {
  return components.find((c) => componentMatches(c, selections)) || null;
}

export function fixedAttributeOptionPrice(components, key, value) {
  for (const component of components) {
    if (key === "Type" && component.attributes.type === value) {
      return component.attributes.typePrice;
    }
    if (key === "Size" && component.attributes.size === value) {
      return component.attributes.sizePrice;
    }
  }
  return 0;
}

export function packageNeedsAttributePicker(components) {
  if (!components || components.length === 0) return false;
  const options = uniqueAttributeOptions(components, ATTRIBUTE_KEYS);
  return ATTRIBUTE_KEYS.some((key) => (options[key] || []).length > 1);
}

export function packageLinePrice(pkg, component) {
  return pkg.price + component.attributes.typePrice + component.attributes.sizePrice;
}

export function resolvePackageCartLine(pkg, component) {
  if (!component) return Object.assign({}, pkg);
  return Object.assign({}, pkg, {
    id: component.goodsId,
    packageId: pkg.id,
    packageName: pkg.name,
    name: pkg.name,
    price: packageLinePrice(pkg, component),
    imagePath: component.imagePath || pkg.imagePath,
    type: MenuGoods.TYPE_GOODS,
    attrType: component.attributes.type,
    attrSize: component.attributes.size,
    attrMeasurement: component.attributes.measurement,
    packageComponents: [],
  });
}

export function attributeDisplayLabel(value, measurement) {
  if (!value) return "";
  if (!measurement) return value;
  return `${value} ${measurement}`;
}
