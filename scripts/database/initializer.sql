DROP TABLE IF EXISTS vendors;

-- // TODO: Agragar campo de contacto a vendedor
-- // TODO: Agregar direccion IP publica y puerto para sistema de compra automatica al servidor del proveedor
-- Se limpian los datos
CREATE TABLE
  vendors (
    vendor_id INTEGER PRIMARY KEY AUTOINCREMENT,
    vendor_name TEXT NOT NULL,
    -- Puede ser almacenado en un buffer de 128 bytes (para almacenar el caracter nulo)
    CHECK (LENGTH (vendor_name) < 128)
  );

INSERT INTO
  vendors (vendor_name)
VALUES
  ('Vendor1'), -- 1
  ('Vendor2'), -- 2
  ('Vendor3');

-- 3
DROP TABLE IF EXISTS products;

-- TODO: Crear campo serial para productos normales
-- Se limpian los datos
CREATE TABLE
  products (
    product_id INTEGER PRIMARY KEY AUTOINCREMENT,
    product_name TEXT NOT NULL,
    product_description TEXT NOT NULL,
    vendor_id INTEGER NOT NULL,
    product_price REAL DEFAULT 0 NOT NULL,
    product_count INTEGER DEFAULT 0 NOT NULL,
    -- Relacion N - 1
    FOREIGN KEY (vendor_id) REFERENCES vendors (vendor_id),
    CHECK (
      LENGTH (product_name) < 128
      AND LENGTH (product_description) < 512
    )
  );

INSERT INTO
  products (
    product_name,
    product_description,
    vendor_id,
    product_price
  )
VALUES
  ('Product1', 'Product1 description', 1, 3.14),
  ('Product2', 'Product2 description', 1, 6.28),
  ('Product3', 'Product3 description', 1, 7.99),
  --
  ('Product4', 'Product4 description', 2, 2.20),
  ('Product5', 'Product5 description', 2, 1.99),
  ('Product6', 'Product6 description', 2, 9.20),
  --
  ('Product7', 'Product7 description', 3, 0.50),
  ('Product8', 'Product8 description', 3, 8.60),
  ('Product9', 'Product9 description', 3, 3.20);

DROP TABLE IF EXISTS smart_products;

-- // TODO: Terminar SmartProduct
CREATE TABLE
  IF NOT EXISTS smart_products (
    smart_product_id INTEGER PRIMARY KEY AUTOINCREMENT,
    smart_product_name TEXT NOT NULL,
    smart_product_description TEXT NOT NULL,
    smart_product_serial TEXT NOT NULL,
    smart_product_inaddr TEXT NOT NULL,
    vendor_id INTEGER NOT NULL,
    smart_product_price REAL DEFAULT 0 NOT NULL,
    FOREIGN KEY (vendor_id) REFERENCES vendors (vendor_id),
    CHECK (
      LENGTH (product_name) < 128
      AND LENGTH (product_description) < 512
      AND LENGTH (product_description) < 512
      AND LENGTH (product_serial) < 128
      AND LENGTH (smart_product_inaddr) < 16 -- xxx.xxx.xxx.xxx
    )
  );

-- Vista para productos estandar
DROP VIEW IF EXISTS products_info;

CREATE VIEW
  products_info AS
SELECT
  product_id,
  product_name,
  product_description,
  vendor_name,
  product_count,
  product_price
FROM
  products as p
  INNER JOIN vendors as v on p.vendor_id = v.vendor_id
ORDER BY
  p.product_price DESC;

-- Vista para productos inteligentes
DROP VIEW IF EXISTS smart_products_info;

CREATE VIEW
  smart_products_info AS
SELECT
  smart_product_id,
  smart_product_name,
  smart_product_description,
  smart_product_serial,
  smart_product_inaddr,
  vendor_id,
  smart_product_price,
FROM
  smart_products as p
  INNER JOIN vendors as v on p.vendor_id = v.vendor_id
ORDER BY
  p.product_price DESC