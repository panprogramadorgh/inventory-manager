DROP TABLE IF EXISTS vendors;

-- Se limpian los datos
CREATE TABLE
  vendors (
    vendor_id INTEGER PRIMARY KEY NOT NULL,
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

-- Se limpian los datos
CREATE TABLE
  products (
    product_id INTEGER PRIMARY KEY NOT NULL,
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
  p.product_price DESC